#include "CommonController.hpp"

#include "SMBR/Exceptions.hpp"
#include "SMBR/Log.hpp"
#include "ControllerUtils.hpp"

#include <sstream>

#include <chrono>

using namespace std::chrono_literals;

CommonController::CommonController(const std::shared_ptr<oatpp::web::mime::ContentMappers>& apiContentMappers,
                                   std::shared_ptr<ISystemModule> systemModule)
    : SMBRControllerBase(apiContentMappers, systemModule)
{}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> CommonController::ping(const std::shared_ptr<IncomingRequest>& request, const oatpp::Enum<dto::ModuleEnum>::AsString& module) {

    return process(__FUNCTION__, [&](){
        std::shared_ptr<ICommonModule> mod;
        if (module == dto::ModuleEnum::pump) {
            auto instanceStr = request->getQueryParameter("instance");
            if (!instanceStr) {
                throw ArgumentException("instance is required for pump module (1-12)");
            }
            int inst;
            try { inst = std::stoi(instanceStr->c_str()); }
            catch (...) { throw ArgumentException("instance must be a number between 1 and 12"); }
            if (inst < 1 || inst > 12) {
                throw ArgumentException("instance must be between 1 and 12");
            }
            Instance instanceEnum = static_cast<Instance>(static_cast<uint8_t>(Instance::Instance_1) + (inst - 1));
            mod = systemModule->commonModule(systemModule->pumpsModule(instanceEnum)->id());
        } else {
            mod = getModule(module);
        }
        float responseTime = waitFor(mod->ping());
        auto pingResponseDto = PingResponseDto::createShared();
        pingResponseDto->time_ms = responseTime;
        return createDtoResponse(Status::CODE_200, pingResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> CommonController::getCoreLoad(const std::shared_ptr<IncomingRequest>& request, const oatpp::Enum<dto::ModuleEnum>::AsString& module) {
    return process(__FUNCTION__, [&](){
        std::shared_ptr<ICommonModule> mod;
        if (module == dto::ModuleEnum::pump) {
            auto instanceStr = request->getQueryParameter("instance");
            if (!instanceStr) {
                throw ArgumentException("instance is required for pump module (1-12)");
            }
            int inst;
            try { inst = std::stoi(instanceStr->c_str()); }
            catch (...) { throw ArgumentException("instance must be a number between 1 and 12"); }
            if (inst < 1 || inst > 12) {
                throw ArgumentException("instance must be between 1 and 12");
            }
            Instance instanceEnum = static_cast<Instance>(static_cast<uint8_t>(Instance::Instance_1) + (inst - 1));
            mod = systemModule->commonModule(systemModule->pumpsModule(instanceEnum)->id());
        } else {
            mod = getModule(module);
        }
        float load = waitFor(mod->getCoreLoad());
        auto loadResponseDto = LoadResponseDto::createShared();
        loadResponseDto->load = load;
        return createDtoResponse(Status::CODE_200, loadResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> CommonController::getCoreTemp(const std::shared_ptr<IncomingRequest>& request, const oatpp::Enum<dto::ModuleEnum>::AsString& module) {
    return process(__FUNCTION__, [&](){
        std::shared_ptr<ICommonModule> mod;
        if (module == dto::ModuleEnum::pump) {
            auto instanceStr = request->getQueryParameter("instance");
            if (!instanceStr) {
                throw ArgumentException("instance is required for pump module (1-12)");
            }
            int inst;
            try { inst = std::stoi(instanceStr->c_str()); }
            catch (...) { throw ArgumentException("instance must be a number between 1 and 12"); }
            if (inst < 1 || inst > 12) {
                throw ArgumentException("instance must be between 1 and 12");
            }
            Instance instanceEnum = static_cast<Instance>(static_cast<uint8_t>(Instance::Instance_1) + (inst - 1));
            mod = systemModule->commonModule(systemModule->pumpsModule(instanceEnum)->id());
        } else {
            mod = getModule(module);
        }
        float temperature = waitFor(mod->getCoreTemp());
        auto tempResponseDto = TempDto::createShared();
        tempResponseDto->temperature = temperature;
        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> CommonController::getBoardTemp(const std::shared_ptr<IncomingRequest>& request, const oatpp::Enum<dto::ModuleEnum>::AsString& module) {
    return process(__FUNCTION__, [&](){
        std::shared_ptr<ICommonModule> mod;
        if (module == dto::ModuleEnum::pump) {
            auto instanceStr = request->getQueryParameter("instance");
            if (!instanceStr) {
                throw ArgumentException("instance is required for pump module (1-12)");
            }
            int inst;
            try { inst = std::stoi(instanceStr->c_str()); }
            catch (...) { throw ArgumentException("instance must be a number between 1 and 12"); }
            if (inst < 1 || inst > 12) {
                throw ArgumentException("instance must be between 1 and 12");
            }
            Instance instanceEnum = static_cast<Instance>(static_cast<uint8_t>(Instance::Instance_1) + (inst - 1));
            mod = systemModule->commonModule(systemModule->pumpsModule(instanceEnum)->id());
        } else {
            mod = getModule(module);
        }
        float temperature = waitFor(mod->getBoardTemp());
        auto tempResponseDto = TempDto::createShared();
        tempResponseDto->temperature = temperature;
        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });
}

static ICommonModule::Ptr byUID(ISystemModule::Ptr s, const oatpp::Enum<dto::ModuleEnum>::AsString& module, 
    const oatpp::Object<ModuleActionRequestDto>& body)
{
    if (!body || !body->uid) {
        throw NotFoundException("uid expected");
    }

    std::string uid = body->uid->c_str();
    Modules type;

    if (module == dto::ModuleEnum::control) {
        type = Modules::Control;
    } else if (module == dto::ModuleEnum::sensor) {
        type = Modules::Sensor;
    } else if (module == dto::ModuleEnum::core) {
        type = Modules::Core;
    } else if (module == dto::ModuleEnum::pump) {
        type = Modules::Pump;
    } else {
        throw NotFoundException("Invalid module type");
    }

    auto existing = s->existing();
    for (const auto& m : existing) {
        if (m.type == type && m.uidHex == uid) {
            return s->commonModule(m);
        }
    }

    std::stringstream ss;
    ss << "Module with uid=" << uid << " and type=" << CommonController::moduleToString(type) << " not found";
    throw NotFoundException(ss.str());
}


std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> CommonController::postRestart(
    const oatpp::data::type::EnumObjectWrapper<dto::ModuleEnum, oatpp::data::type::EnumInterpreterAsString<dto::ModuleEnum, false>>& module,
    const oatpp::Object<ModuleActionRequestDto>& body) {

    return processBool(__FUNCTION__, [&](){
        auto m = byUID(systemModule, module, body);
        return waitFor(m->restartModule());
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> CommonController::postUsbBootloader(
    const oatpp::Enum<dto::ModuleEnum>::AsString& module,
    const oatpp::Object<ModuleActionRequestDto>& body) {

    return processBool(__FUNCTION__, [&](){
        auto m = byUID(systemModule, module, body);
        return waitFor(m->rebootModuleUsbBootloader());
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> CommonController::postCanBootloader(
    const oatpp::Enum<dto::ModuleEnum>::AsString& module,
    const oatpp::Object<ModuleActionRequestDto>& body) {

    return processBool(__FUNCTION__, [&](){
        auto m = byUID(systemModule, module, body);
        return waitFor(m->rebootModuleCanBootloader());
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> CommonController::getFwVersion(const std::shared_ptr<IncomingRequest>& request, const oatpp::Enum<dto::ModuleEnum>::AsString& module) {
    return process(__FUNCTION__, [&]() {
        std::shared_ptr<ICommonModule> mod;
        if (module == dto::ModuleEnum::pump) {
            auto instanceStr = request->getQueryParameter("instance");
            if (!instanceStr) {
                throw ArgumentException("instance is required for pump module (1-12)");
            }
            int inst;
            try { inst = std::stoi(instanceStr->c_str()); }
            catch (...) { throw ArgumentException("instance must be a number between 1 and 12"); }
            if (inst < 1 || inst > 12) {
                throw ArgumentException("instance must be between 1 and 12");
            }
            Instance instanceEnum = static_cast<Instance>(static_cast<uint8_t>(Instance::Instance_1) + (inst - 1));
            mod = systemModule->commonModule(systemModule->pumpsModule(instanceEnum)->id());
        } else {
            mod = getModule(module);
        }
        auto fw = waitFor(mod->getFwVersion());
        auto dto = VersionDto::createShared();
        dto->version = fw.version.c_str();
        dto->hash = fw.hash.c_str();
        dto->dirty = fw.dirty;
        return createDtoResponse(Status::CODE_200, dto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> CommonController::getHwVersion(const std::shared_ptr<IncomingRequest>& request, const oatpp::Enum<dto::ModuleEnum>::AsString& module) {
    return process(__FUNCTION__, [&]() {
        std::shared_ptr<ICommonModule> mod;
        if (module == dto::ModuleEnum::pump) {
            auto instanceStr = request->getQueryParameter("instance");
            if (!instanceStr) throw ArgumentException("instance is required for pump module (1-12)");
            int inst;
            try { inst = std::stoi(instanceStr->c_str()); }
            catch (...) { throw ArgumentException("instance must be a number between 1 and 12"); }
            if (inst < 1 || inst > 12) throw ArgumentException("instance must be between 1 and 12");
            Instance instanceEnum = static_cast<Instance>(static_cast<uint8_t>(Instance::Instance_1) + (inst - 1));
            mod = systemModule->commonModule(systemModule->pumpsModule(instanceEnum)->id());
        } else {
            mod = getModule(module);
        }
        auto version = waitFor(mod->getHwVersion());
        auto dto = HwVersionDto::createShared();
        dto->version = version;
        return createDtoResponse(Status::CODE_200, dto);
    });
}
