#include "PumpsController.hpp"

#include "SMBR/Exceptions.hpp"
#include "SMBR/Log.hpp"
#include "ControllerUtils.hpp"

#include <chrono>

using namespace std::chrono_literals;

PumpsController::PumpsController(const std::shared_ptr<oatpp::web::mime::ContentMappers>& apiContentMappers,
                                 std::shared_ptr<ISystemModule> systemModule)
    : SMBRControllerBase(apiContentMappers, systemModule)
{}

uint8_t PumpsController::pumpCountForInstance(Instance instance) {
    return waitFor(systemModule->pumpsModule(instance)->getPumpCount());
}

void PumpsController::validatePumpIndex(Instance instance, uint8_t pump_index) {
    uint8_t count = pumpCountForInstance(instance);
    if (pump_index < 1 || pump_index > count) {
        throw ArgumentException("Invalid pump_index. Must be between 1 and " + std::to_string(count) + ".");
    }
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> PumpsController::getPumpCount(const UInt8& instance_index) {
    return process(__FUNCTION__, [&](){
        if (instance_index < 1 || instance_index > 12) {
            throw ArgumentException("Invalid instance_index. Must be between 1 and 12.");
        }
        
        Instance instance = static_cast<Instance>(static_cast<uint8_t>(Instance::Instance_1) + (instance_index - 1));
        
        auto pumpCount = waitFor(systemModule->pumpsModule(instance)->getPumpCount());
        
        auto responseDto = PumpCountDto::createShared();
        responseDto->pump_count = pumpCount;
        return createDtoResponse(Status::CODE_200, responseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> PumpsController::getPumpInfo(const UInt8& instance_index, const UInt8& pump_index) {
    return process(__FUNCTION__, [&](){
        if (instance_index < 1 || instance_index > 12) {
            throw ArgumentException("Invalid instance_index. Must be between 1 and 12.");
        }
        
        Instance instance = static_cast<Instance>(static_cast<uint8_t>(Instance::Instance_1) + (instance_index - 1));
        validatePumpIndex(instance, pump_index);
        
        auto pumpInfo = waitFor(systemModule->pumpsModule(instance)->getPumpInfo(pump_index));
        
        auto responseDto = PumpInfoDto::createShared();
        responseDto->max_flowrate = pumpInfo.max_flowrate;
        responseDto->min_flowrate = pumpInfo.min_flowrate;
        return createDtoResponse(Status::CODE_200, responseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> PumpsController::getPumpSpeed(const UInt8& instance_index, const UInt8& pump_index) {
    return process(__FUNCTION__, [&](){
        if (instance_index < 1 || instance_index > 12) {
            throw ArgumentException("Invalid instance_index. Must be between 1 and 12.");
        }
        
        Instance instance = static_cast<Instance>(static_cast<uint8_t>(Instance::Instance_1) + (instance_index - 1));
        validatePumpIndex(instance, pump_index);
        
        auto speed = waitFor(systemModule->pumpsModule(instance)->getSpeed(pump_index));
        
        auto responseDto = SpeedDto::createShared();
        responseDto->speed = speed;
        return createDtoResponse(Status::CODE_200, responseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> PumpsController::setPumpSpeed(const UInt8& instance_index, const UInt8& pump_index, const oatpp::Object<SpeedDto>& body) {
    return processBool(__FUNCTION__, [&](){
        if (instance_index < 1 || instance_index > 12) {
            throw ArgumentException("Invalid instance_index. Must be between 1 and 12.");
        }
        
        Instance instance = static_cast<Instance>(static_cast<uint8_t>(Instance::Instance_1) + (instance_index - 1));
        validatePumpIndex(instance, pump_index);
        
        if (!body) {
            throw ArgumentException("Request body is required.");
        }
        
        if (!body->speed) {
            throw ArgumentException("Speed field is required.");
        }
        
        float speedValue = body->speed;
        if (speedValue < -1.0f || speedValue > 1.0f) {
            throw ArgumentException("Invalid speed value. Must be between -1.0 and 1.0.");
        }
        
        return waitFor(systemModule->pumpsModule(instance)->setSpeed(pump_index, speedValue));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> PumpsController::getPumpFlowrate(const UInt8& instance_index, const UInt8& pump_index) {
    return process(__FUNCTION__, [&](){
        if (instance_index < 1 || instance_index > 12) {
            throw ArgumentException("Invalid instance_index. Must be between 1 and 12.");
        }
        
        Instance instance = static_cast<Instance>(static_cast<uint8_t>(Instance::Instance_1) + (instance_index - 1));
        validatePumpIndex(instance, pump_index);
        
        auto flowrate = waitFor(systemModule->pumpsModule(instance)->getFlowrate(pump_index));
        
        auto responseDto = FlowrateDto::createShared();
        responseDto->flowrate = flowrate;
        return createDtoResponse(Status::CODE_200, responseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> PumpsController::setPumpFlowrate(const UInt8& instance_index, const UInt8& pump_index, const oatpp::Object<FlowrateDto>& body) {
    return processBool(__FUNCTION__, [&](){
        if (instance_index < 1 || instance_index > 12) {
            throw ArgumentException("Invalid instance_index. Must be between 1 and 12.");
        }
        
        Instance instance = static_cast<Instance>(static_cast<uint8_t>(Instance::Instance_1) + (instance_index - 1));
        validatePumpIndex(instance, pump_index);
        
        if (!body) {
            throw ArgumentException("Request body is required.");
        }
        
        if (!body->flowrate) {
            throw ArgumentException("Flowrate field is required.");
        }
        
        float flowrateValue = body->flowrate;
        if (flowrateValue < -1000.0f || flowrateValue > 1000.0f) {
            throw ArgumentException("Invalid flowrate value. Must be between -1000 and 1000.");
        }
        
        return waitFor(systemModule->pumpsModule(instance)->setFlowrate(pump_index, flowrateValue));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> PumpsController::calibratePump(const UInt8& instance_index, const UInt8& pump_index, const oatpp::Object<FlowrateDto>& body) {
    return processBool(__FUNCTION__, [&](){
        if (instance_index < 1 || instance_index > 12) {
            throw ArgumentException("Invalid instance_index. Must be between 1 and 12.");
        }

        Instance instance = static_cast<Instance>(static_cast<uint8_t>(Instance::Instance_1) + (instance_index - 1));
        validatePumpIndex(instance, pump_index);

        if (!body) {            throw ArgumentException("Request body is required.");
        }

        if (!body->flowrate) {
            throw ArgumentException("Flowrate field is required.");
        }

        float flowrateValue = body->flowrate;
        if (flowrateValue < 0.0f || flowrateValue > 1000.0f) {
            throw ArgumentException("Invalid flowrate value. Must be between 0 and 1000.");
        }

        return waitFor(systemModule->pumpsModule(instance)->setMaxFlowrate(pump_index, flowrateValue));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> PumpsController::movePump(const UInt8& instance_index, const UInt8& pump_index, const oatpp::Object<MoveDto>& body) {
    return processBool(__FUNCTION__, [&](){
        if (instance_index < 1 || instance_index > 12) {
            throw ArgumentException("Invalid instance_index. Must be between 1 and 12.");
        }
        
        Instance instance = static_cast<Instance>(static_cast<uint8_t>(Instance::Instance_1) + (instance_index - 1));
        validatePumpIndex(instance, pump_index);
        
        if (!body) {
            throw ArgumentException("Request body is required.");
        }
        
        if (!body->volume) {
            throw ArgumentException("Volume field is required.");
        }
        
        if (!body->flowrate) {
            throw ArgumentException("Flowrate field is required.");
        }
        
        float volumeValue = body->volume;
        if (volumeValue < -1000.0f || volumeValue > 1000.0f) {
            throw ArgumentException("Invalid volume value. Must be between -1000 and 1000.");
        }
        
        float flowrateValue = body->flowrate;
        if (flowrateValue < 0.0f || flowrateValue > 1000.0f) {
            throw ArgumentException("Invalid flowrate value. Must be between 0 and 1000.");
        }
        
        return waitFor(systemModule->pumpsModule(instance)->move(pump_index, volumeValue, flowrateValue));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> PumpsController::stopPump(const UInt8& instance_index, const UInt8& pump_index) {
    return processBool(__FUNCTION__, [&](){
        if (instance_index < 1 || instance_index > 12) {
            throw ArgumentException("Invalid instance_index. Must be between 1 and 12.");
        }
        
        Instance instance = static_cast<Instance>(static_cast<uint8_t>(Instance::Instance_1) + (instance_index - 1));
        validatePumpIndex(instance, pump_index);
        
        return waitFor(systemModule->pumpsModule(instance)->stop(pump_index));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> PumpsController::setPumpInstance(const String& uid, const UInt8& instance_index) {
    return processBool(__FUNCTION__, [&](){
        if (instance_index < 1 || instance_index > 12) {
            throw ArgumentException("Invalid instance_index. Must be between 1 and 12.");
        }

        std::string uidStr = uid->c_str();
        auto existing = systemModule->existing();
        Instance moduleInstance;
        bool found = false;

        for (const auto& m : existing) {
            if (m.type == Modules::Pump && m.uidHex == uidStr) {
                moduleInstance = m.instance;
                found = true;
                break;
            }
        }

        if (!found) {
            throw NotFoundException("Pump module with specified UID not found");
        }

        return waitFor(systemModule->pumpsModule(moduleInstance)->setInstance(instance_index));
    });
}
