#include "SystemController.hpp"

#include "SMBR/Exceptions.hpp"
#include "SMBR/Log.hpp"
#include "ControllerUtils.hpp"

#include <sstream>
#include <optional>
#include <unordered_set>

#include <chrono>

using namespace std::chrono_literals;

SystemController::SystemController(const std::shared_ptr<oatpp::web::mime::ContentMappers>& apiContentMappers,
                                   std::shared_ptr<ISystemModule> systemModule)
    : SMBRControllerBase(apiContentMappers, systemModule)
{}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SystemController::getSystemModules() {
    LDEBUG("API") << "Api getSystemModules begin" << LE;
    auto dtoList = oatpp::List<oatpp::Object<ModuleInfoDto>>::createShared();

    auto result = waitFor(systemModule->getAvailableModules());

    for (auto m : result){
        auto moduleInfoDto = ModuleInfoDto::createShared();

        if (m.type == Modules::Core) {
            moduleInfoDto->module_type = "core";
        } else if (m.type == Modules::Control) {
            moduleInfoDto->module_type = "control";
        } else if (m.type == Modules::Sensor) {
            moduleInfoDto->module_type = "sensor";
        } else if (m.type == Modules::Pump) {
            moduleInfoDto->module_type = "pump";
        } else {
            moduleInfoDto->module_type = "unknown";
        }

        moduleInfoDto->uid = m.uidHex;
        moduleInfoDto->instance = instanceToString(m.instance);
        dtoList->push_back(moduleInfoDto);
    }

    LDEBUG("API") << "Api getSystemModules end" << LE;
    return createDtoResponse(Status::CODE_200, dtoList);
}
std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SystemController::getSystemErrors() {
    return process(__FUNCTION__, [&](){
        auto result = waitFor(systemModule->getAvailableModules());
        auto errors = oatpp::List<oatpp::Object<SystemProblemDto>>::createShared();

        // ModuleUnavailable (ID=1)
        std::unordered_set<std::string> foundModules;
        for (auto &m : result) {
            foundModules.insert(moduleToString(m.type));
        }
        std::vector<std::string> requiredModules = {"core", "sensor", "control"};
        for (auto &req : requiredModules) {
            if (foundModules.find(req) == foundModules.end()) {
                auto err = SystemProblemDto::createShared();
                err->type = "ModuleUnavailable";
                err->id = 1;
                err->message = "One or more modules are unavailable";
                err->detail = "Module " + req + " not responding.";
                errors->push_back(err);
            }
        }

        // UnknownInstance (ID=2)
        for (auto &m : result) {
            std::string inst = instanceToString(m.instance);
            if (inst == "All" || inst == "Undefined" || inst == "Reserved") {
                auto err = SystemProblemDto::createShared();
                err->type = "UnknownInstance";
                err->id = 2;
                err->message = "Unknown module instance detected: " + inst;
                err->detail = "Module " + moduleToString(m.type) + " reported instance value " + inst + ".";
                errors->push_back(err);
            }
        } 

        // DuplicateInstance (ID=3) 
        std::unordered_map<std::string, std::unordered_map<std::string, int>> moduleInstanceCount;
        for (auto &m : result) {
            std::string moduleType = moduleToString(m.type);
            std::string inst = instanceToString(m.instance);
            moduleInstanceCount[moduleType][inst]++;
        }
        for (const auto &modPair : moduleInstanceCount) {
            for (const auto &kv : modPair.second) {
                if (kv.second > 1) {
                    auto err = SystemProblemDto::createShared();
                    err->type = "DuplicateInstance";
                    err->id = 3;
                    err->message = "Multiple instances of the same module instance detected: " + kv.first;
                    err->detail = "Detected " + std::to_string(kv.second) + "x " + kv.first +
                                  " in module type " + modPair.first + ".";
                    errors->push_back(err);
                }
            }
        } 

        auto resp = SystemProblemResponseDto::createShared();
        if (errors->empty()) {
            resp->message = "System is operating normally. No errors detected.";
            resp->problems = {};
        } else {
            resp->message = "System errors detected. See details in 'problems'.";
            resp->problems = errors;
        }
        return createDtoResponse(Status::CODE_200, resp);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SystemController::getSystemWarnings() {
    return process(__FUNCTION__, [&](){
        auto result = waitFor(systemModule->getAvailableModules());
        auto warnings = oatpp::List<oatpp::Object<SystemProblemDto>>::createShared();

        static std::optional<std::string> nonCoreFwRef;
        for (auto &m : result) {
            std::string moduleType = moduleToString(m.type);
            dto::ModuleEnum moduleEnum;
            switch(m.type) {
            case Modules::Core:    
                moduleEnum = dto::ModuleEnum::core; break;
            case Modules::Control: 
                moduleEnum = dto::ModuleEnum::control; break;
            case Modules::Sensor:  
                moduleEnum = dto::ModuleEnum::sensor; break;
            default:
                // OATPP_LOGw("SMBRController", ("Unknown module type: " + std::to_string((int)m.type)).c_str());
                continue;
            }
            auto fwDto = waitFor(getModule(oatpp::Enum<dto::ModuleEnum>::AsString(moduleEnum))->getFwVersion());

            std::string fwVersion = fwDto.version;
            bool isDirty = fwDto.dirty;

            // FirmwareVersionMismatc (ID=1) 
            if (m.type != Modules::Core) {
                if (!nonCoreFwRef.has_value()) {
                    nonCoreFwRef = fwVersion;
                } 
                else if (nonCoreFwRef.value() != fwVersion) {
                    auto warn = SystemProblemDto::createShared();
                    warn->type = "FirmwareVersionMismatch";
                    warn->id = 1;
                    warn->message = "Firmware versions differ between modules of different types (excluding core modules)";
                    warn->detail = "Expected " + nonCoreFwRef.value() + 
                                " but got " + fwVersion + " for " + moduleType;
                    warnings->push_back(warn);
                }
            }

            // DirtyBuildFirmware (ID=2) 
            if (isDirty) {
                auto warn = SystemProblemDto::createShared();
                warn->type = "DirtyBuildFirmware";
                warn->id = 2;
                warn->message = "Dirty build firmware detected on a module";
                warn->detail = "Module " + moduleType + " flagged as dirty build";
                warnings->push_back(warn);
            }

            // ModuleHighPing (ID=5) 
            float pingTime = waitFor(getModule(oatpp::Enum<dto::ModuleEnum>::AsString(moduleEnum))->ping());
            if (pingTime > 500.0f) {
                auto warn = SystemProblemDto::createShared();
                warn->id = 5;
                warn->type = "ModuleHighPing";
                warn->message = "High ping time detected on a module";
                warn->detail = "Module " + moduleType + " responded in " + std::to_string(pingTime) + " ms";
                warnings->push_back(warn);
            }
        }

        // CANBusErrorRateHigh (ID=3, ID=4) 
        try {
            uint64_t rxPackets = readCanValue("rx_packets");
            uint64_t txPackets = readCanValue("tx_packets");
            uint64_t rxErrors  = readCanValue("rx_errors");
            uint64_t txErrors  = readCanValue("tx_errors");
            uint64_t rxDropped = readCanValue("rx_dropped");
            uint64_t txDropped = readCanValue("tx_dropped");
            uint64_t collisions= readCanValue("collisions");

            uint64_t totalPackets = rxPackets + txPackets;
            uint64_t totalErrors  = rxErrors + txErrors + rxDropped + txDropped + collisions;

            if (totalPackets == 0) {
                auto warn = SystemProblemDto::createShared();
                warn->id = 3;
                warn->type = "CANBusUnreachable";
                warn->message = "No packets observed on CAN bus (possibly unreachable)";
                warn->detail = "Interface can0 has 0 transmitted/received packets";
                warnings->push_back(warn);
            } else {
                double errorRate = (static_cast<double>(totalErrors) / totalPackets) * 100.0;
                if (errorRate > 5.0) {
                    auto warn = SystemProblemDto::createShared();
                    warn->id = 4;
                    warn->type = "CANBusErrorRateHigh";
                    warn->message = "CAN bus error rate above threshold";
                    warn->detail = "Error rate at " + std::to_string(errorRate) + " % on CAN interface can0";
                    warnings->push_back(warn);
                }
            }
        } catch (const std::exception& ex) {
            auto warn = SystemProblemDto::createShared();
            warn->id = 3;
            warn->type = "CANBusStatReadError";
            warn->message = "Failed to read CAN bus statistics";
            warn->detail = ex.what();
            warnings->push_back(warn);
        }

        auto resp = SystemProblemResponseDto::createShared();
            if (warnings->empty()) {
            resp->message = "System is operating normally. No errors detected.";
            resp->problems = {};
        } else {
            resp->message = "System warnings detected. See details in 'problems'.";
            resp->problems = warnings;
        }
        return createDtoResponse(Status::CODE_200, resp);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SystemController::getModuleIssues() {
    auto listDto = ModuleIssuesListDto::createShared();
    auto activeIssues = systemModule->issues()->getActiveIssuesData();
    if (activeIssues.empty()) {
        listDto->message = "No active module issues detected. System is operating normally.";
        listDto->issues = {};
    } else {
        listDto->message = "Active issues detected. See 'issues' field for details.";
        listDto->issues = oatpp::List<oatpp::Object<ModuleIssueDto>>::createShared();
        for (const auto& i : activeIssues) {
            auto dto = ModuleIssueDto::createShared();
            dto->id = i.error_type;
            dto->name = i.name;
            dto->index = i.index;
            dto->timestamp = i.timestamp;
            dto->value = i.value;
            dto->module = moduleToString(i.module.type);
            dto->instance = instanceToString(i.module.instance);
            listDto->issues->push_back(dto);
        }
    }
    return createDtoResponse(Status::CODE_200, listDto);
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SystemController::getCanRxPackets() {
    return process(__FUNCTION__, [&]() {
        return readCanStat<RxPacketsDto>("rx_packets", "rx_packets",
            [](uint64_t val){
                auto dto = RxPacketsDto::createShared();
                dto->rx_packets = val;
                return dto;
            });
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response>  SystemController::getCanTxPackets() {
    return process(__FUNCTION__, [&]() {
        return readCanStat<TxPacketsDto>("tx_packets", "tx_packets",
            [](uint64_t val){
                auto dto = TxPacketsDto::createShared();
                dto->tx_packets = val;
                return dto;
            });
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response>  SystemController::getCanRxErrors() {
    return process(__FUNCTION__, [&]() {
        return readCanStat<RxErrorsDto>("rx_errors", "rx_errors",
            [](uint64_t val){
                auto dto = RxErrorsDto::createShared();
                dto->rx_errors = val;
                return dto;
            });
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response>  SystemController::getCanTxErrors() {
    return process(__FUNCTION__, [&]() {
        return readCanStat<TxErrorsDto>("tx_errors", "tx_errors",
            [](uint64_t val){
                auto dto = TxErrorsDto::createShared();
                dto->tx_errors = val;
                return dto;
            });
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response>  SystemController::getCanRxDropped() {
    return process(__FUNCTION__, [&]() {
        return readCanStat<RxDroppedDto>("rx_dropped", "rx_dropped",
            [](uint64_t val){
                auto dto = RxDroppedDto::createShared();
                dto->rx_dropped = val;
                return dto;
            });
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response>  SystemController::getCanTxDropped() {
    return process(__FUNCTION__, [&]() {
        return readCanStat<TxDroppedDto>("tx_dropped", "tx_dropped",
            [](uint64_t val){
                auto dto = TxDroppedDto::createShared();
                dto->tx_dropped = val;
                return dto;
            });
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response>  SystemController::getCanCollisions() {
    return process(__FUNCTION__, [&]() {
        return readCanStat<CollisionsDto>("collisions", "collisions",
            [](uint64_t val){
                auto dto = CollisionsDto::createShared();
                dto->collisions = val;
                return dto;
            });
    });
}

uint64_t SystemController::readCanValue(const std::string& statName) {
    using namespace std::chrono_literals;

    auto future = std::async(std::launch::async, [statName]() -> uint64_t {
        std::string path = "/sys/class/net/can0/statistics/" + statName;
        std::ifstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open CAN stat file: " + path);
        }
        uint64_t value;
        file >> value;
        if (file.fail()) {
            throw std::runtime_error("Failed to read value from CAN stat file: " + path);
        }
        return value;
    });

    if (future.wait_for(2s) == std::future_status::ready) {
        return future.get();
    } else {
        throw TimeoutException("Timeout reading CAN stat: " + statName);
    }
}
