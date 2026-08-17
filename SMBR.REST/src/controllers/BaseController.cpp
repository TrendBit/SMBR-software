#include "BaseController.hpp"

#include "SMBR/Exceptions.hpp"
#include "SMBR/Log.hpp"

#include <string>

SMBRControllerBase::SMBRControllerBase(const std::shared_ptr<oatpp::web::mime::ContentMappers>& apiContentMappers,
                                       std::shared_ptr<ISystemModule> systemModule)
    : oatpp::web::server::api::ApiController(apiContentMappers)
    , systemModule(systemModule)
{
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRControllerBase::process(
    std::string name,
    std::function<std::shared_ptr<oatpp::web::protocol::http::outgoing::Response>()> body
){
    try {
        LDEBUG("API") << "Api " << name << " begin" << LE;
        auto b = body();
        LDEBUG("API") << "Api " << name << " end" << LE;
        return b;
    } catch (TimeoutException & e){
        auto dto = MessageDto::createShared();
        dto->message = name + " timed out: " + std::string(e.what());
        LWARNING("API") << "Api " << name << " failed with TimeoutException " << std::string(e.what()) << LE;
        return createDtoResponse(Status::CODE_504, dto);
    } catch (NotFoundException & e){
        auto dto = MessageDto::createShared();
        dto->message = name + " not found: " + std::string(e.what());
        LWARNING("API") << "Api " << name << " failed with NotFoundException " << std::string(e.what()) << LE;
        return createDtoResponse(Status::CODE_404, dto);
    } catch (ArgumentException & e){
        auto dto = MessageDto::createShared();
        dto->message = "Invalid request body: " + std::string(e.what());
        LWARNING("API") << "Api " << name << " failed with ArgumentException " << std::string(e.what()) << LE;
        return createDtoResponse(Status::CODE_400, dto);
    } catch (ConflictException & e){
        auto dto = MessageDto::createShared();
        dto->message = name + " conflict: " + std::string(e.what());
        LWARNING("API") << "Api " << name << " failed with ConflictException " << std::string(e.what()) << LE;
        return createDtoResponse(Status::CODE_409, dto);
    } catch (std::exception & e){
        auto dto = MessageDto::createShared();
        dto->message = "Failed to retrieve " + name + ": " + std::string(e.what());
        LWARNING("API") << "Api " << name << " failed with Exception " << std::string(e.what()) << LE;
        return createDtoResponse(Status::CODE_500, dto);
    }
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRControllerBase::processBool(
    std::string name,
    std::function<bool()> body
){
    try {
        LDEBUG("API") << "Api " << name << " begin" << LE;
        bool success = body();
        auto dto = MessageDto::createShared();
        if (success) {
            dto->message = name + " successful";
            LDEBUG("API") << "Api " << name << " end (return value = true)" << LE;
            return createDtoResponse(Status::CODE_200, dto);
        } else {
            dto->message = name + " failed";
            LDEBUG("API") << "Api " << name << " end (return value = false)" << LE;
            return createDtoResponse(Status::CODE_500, dto);
        }
    } catch (TimeoutException & e){
        auto dto = MessageDto::createShared();
        dto->message = name + " timed out: " + std::string(e.what());
        LWARNING("API") << "Api " << name << " failed with TimeoutException " << std::string(e.what()) << LE;
        return createDtoResponse(Status::CODE_504, dto);
    } catch (NotFoundException & e){
        auto dto = MessageDto::createShared();
        dto->message = name + " not found: " + std::string(e.what());
        LWARNING("API") << "Api " << name << " failed with NotFoundException " << std::string(e.what()) << LE;
        return createDtoResponse(Status::CODE_404, dto);
    } catch (ArgumentException & e){
        auto dto = MessageDto::createShared();
        dto->message = "Invalid request body: " + std::string(e.what());
        LWARNING("API") << "Api " << name << " failed with ArgumentException " << std::string(e.what()) << LE;
        return createDtoResponse(Status::CODE_400, dto);
    } catch (ConflictException & e){
        auto dto = MessageDto::createShared();
        dto->message = name + " conflict: " + std::string(e.what());
        LWARNING("API") << "Api " << name << " failed with ConflictException " << std::string(e.what()) << LE;
        return createDtoResponse(Status::CODE_409, dto);
    } catch (std::exception & e){
        auto dto = MessageDto::createShared();
        dto->message = "Failed to retrieve " + name + ": " + std::string(e.what());
        LWARNING("API") << "Api " << name << " failed with Exception " << std::string(e.what()) << LE;
        return createDtoResponse(Status::CODE_500, dto);
    }
}

std::string SMBRControllerBase::instanceToString(Instance instance) {
    switch (instance) {
        case Instance::Undefined: return "Undefined";
        case Instance::Exclusive: return "Exclusive";
        case Instance::All: return "All";
        case Instance::Reserved: return "Reserved";
        case Instance::Instance_1: return "Instance_1";
        case Instance::Instance_2: return "Instance_2";
        case Instance::Instance_3: return "Instance_3";
        case Instance::Instance_4: return "Instance_4";
        case Instance::Instance_5: return "Instance_5";
        case Instance::Instance_6: return "Instance_6";
        case Instance::Instance_7: return "Instance_7";
        case Instance::Instance_8: return "Instance_8";
        case Instance::Instance_9: return "Instance_9";
        case Instance::Instance_10: return "Instance_10";
        case Instance::Instance_11: return "Instance_11";
        case Instance::Instance_12: return "Instance_12";
        default: return "Unknown";
    }
}

std::string SMBRControllerBase::moduleToString(Modules module) {
    switch (module) {
        case Modules::Core: return "core";
        case Modules::Control: return "control";
        case Modules::Sensor: return "sensor";
        case Modules::Pump: return "pump";
        case Modules::Unknown: return "Unknown";
        default: return "Invalid";
    }
}


std::shared_ptr<ICommonModule> SMBRControllerBase::getModule(const oatpp::Enum<dto::ModuleEnum>::AsString& module){
    if (module == dto::ModuleEnum::control) {
        return systemModule->commonModule(systemModule->controlModule()->id());
    } else if (module == dto::ModuleEnum::sensor) {
        return systemModule->commonModule(systemModule->sensorModule()->id());
    } else if (module == dto::ModuleEnum::core) {
        return systemModule->commonModule(systemModule->coreModule()->id());
    }
    throw NotFoundException("Module not found");
}
