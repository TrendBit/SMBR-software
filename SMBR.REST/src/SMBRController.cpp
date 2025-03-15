#include "SMBRController.hpp"
#include "SMBR/Exceptions.hpp"
#include <Poco/DateTimeFormatter.h>

#include "SMBR/Recipes.hpp"
#include "SMBR/Scheduler.hpp"

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::process(
    std::string name,
    std::function<std::shared_ptr<oatpp::web::protocol::http::outgoing::Response>()> body
){
    try {
        return body();
    } catch (TimeoutException & e){
        auto dto = MessageDto::createShared();
        dto->message = name + " timed out: " + std::string(e.what());
        return createDtoResponse(Status::CODE_504, dto);
    } catch (NotFoundException & e){
        auto dto = MessageDto::createShared();
        dto->message = name + " not found: " + std::string(e.what());
        return createDtoResponse(Status::CODE_404, dto);
    } catch (std::exception & e){
        auto dto = MessageDto::createShared();
        dto->message = "Failed to retrieve " + name + ": " + std::string(e.what());
        return createDtoResponse(Status::CODE_500, dto);
    }
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::processBool(
    std::string name,
    std::function<bool()> body
){
    try {
        bool success = body();
        auto dto = MessageDto::createShared();
        if (success) {
            dto->message = name + " successful";
            return createDtoResponse(Status::CODE_200, dto);
        } else {
            dto->message = name + " failed";
            return createDtoResponse(Status::CODE_500, dto);
        }
    } catch (TimeoutException & e){
        auto dto = MessageDto::createShared();
        dto->message = name + " timed out: " + std::string(e.what());
        return createDtoResponse(Status::CODE_504, dto);
    } catch (NotFoundException & e){
        auto dto = MessageDto::createShared();
        dto->message = name + " not found: " + std::string(e.what());
        return createDtoResponse(Status::CODE_404, dto);
    } catch (std::exception & e){
        auto dto = MessageDto::createShared();
        dto->message = "Failed to retrieve " + name + ": " + std::string(e.what());
        return createDtoResponse(Status::CODE_500, dto);
    }
}

template <class T>
T wait(std::future<T> future){
    future.wait();
    return future.get();
}

SMBRController::SMBRController(const std::shared_ptr<oatpp::web::mime::ContentMappers>& apiContentMappers,
                           std::shared_ptr<ISystemModule> systemModule)
    : oatpp::web::server::api::ApiController(apiContentMappers)
    , systemModule(systemModule){
        scheduler_ = std::make_shared<Scheduler>(systemModule);
        recipes_ = std::make_shared<Recipes>("/data/recipes/");
    }

  // ==========================================
  // System Endpoints
  // ==========================================

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getSystemModules() {
    auto dtoList = oatpp::List<oatpp::Object<ModuleInfoDto>>::createShared();

    auto future = systemModule->getAvailableModules();        

    future.wait();
    auto result = future.get();

    for (auto m : result){
        auto moduleInfoDto = ModuleInfoDto::createShared();
        
        if (m.type == Modules::Core) {
            moduleInfoDto->module_type = "core";
        } else if (m.type == Modules::Control) {
            moduleInfoDto->module_type = "control";
        } else if (m.type == Modules::Sensor) {
            moduleInfoDto->module_type = "sensor";
        } else {
            moduleInfoDto->module_type = "unknown";
        }
        
        moduleInfoDto->uid = m.uidHex;
        moduleInfoDto->instance = instanceToString(m.instance); 
        dtoList->push_back(moduleInfoDto);
    }

    return createDtoResponse(Status::CODE_200, dtoList);
}

std::string SMBRController::instanceToString(Instance instance) {
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

  // ==========================================
  // Common Endpoints
  // ==========================================

std::shared_ptr<ICommonModule> SMBRController::getModule(const oatpp::Enum<dto::ModuleEnum>::AsString& module){
    if (module == dto::ModuleEnum::control) {
        return systemModule->commonModule(systemModule->controlModule()->id());
    } else if (module == dto::ModuleEnum::sensor) {
        return systemModule->commonModule(systemModule->sensorModule()->id());
    } else if (module == dto::ModuleEnum::core) {
        return systemModule->commonModule(systemModule->coreModule()->id());
    }
    throw NotFoundException("Module not found");
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::ping(const oatpp::Enum<dto::ModuleEnum>::AsString& module) {

    return process(__FUNCTION__, [&](){
        float responseTime = wait(getModule(module)->ping());
        auto pingResponseDto = PingResponseDto::createShared();
        pingResponseDto->time_ms = responseTime;
        return createDtoResponse(Status::CODE_200, pingResponseDto);      
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getCoreLoad(const oatpp::Enum<dto::ModuleEnum>::AsString& module) {
    return process(__FUNCTION__, [&](){
        float load = wait(getModule(module)->getCoreLoad()); 
        auto loadResponseDto = LoadResponseDto::createShared();
        loadResponseDto->load = load;
        return createDtoResponse(Status::CODE_200, loadResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getCoreTemp(const oatpp::Enum<dto::ModuleEnum>::AsString& module) {
    return process(__FUNCTION__, [&](){
        float temperature = wait(getModule(module)->getCoreTemp());
        auto tempResponseDto = TempDto::createShared();
        tempResponseDto->temperature = temperature;
        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getBoardTemp(const oatpp::Enum<dto::ModuleEnum>::AsString& module) {
    return process(__FUNCTION__, [&](){
        float temperature = wait(getModule(module)->getBoardTemp());
        auto tempResponseDto = TempDto::createShared();
        tempResponseDto->temperature = temperature;
        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });
}

static ICommonModule::Ptr byUID(ISystemModule::Ptr s, const oatpp::data::type::EnumObjectWrapper<dto::ModuleEnum, oatpp::data::type::EnumInterpreterAsString<dto::ModuleEnum, false>>& module,
    const oatpp::Object<ModuleActionRequestDto>& body){
    if (!body || !body->uid) {
        throw NotFoundException("uid expected");
    }    
    ModuleID moduleID;
    moduleID.uidHex = body->uid->c_str();


    if (module == dto::ModuleEnum::control) {
        moduleID.type == Modules::Control;        
    } else if (module == dto::ModuleEnum::sensor) {
        moduleID.type == Modules::Sensor;
    } else if (module == dto::ModuleEnum::core) {
        moduleID.type == Modules::Core;
    } 
    return s->commonModule(moduleID);
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::postRestart(
    const oatpp::data::type::EnumObjectWrapper<dto::ModuleEnum, oatpp::data::type::EnumInterpreterAsString<dto::ModuleEnum, false>>& module,
    const oatpp::Object<ModuleActionRequestDto>& body) {

    return processBool(__FUNCTION__, [&](){
        auto m = byUID(systemModule, module, body);
        return wait(m->restartModule());
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::postUsbBootloader(
    const oatpp::Enum<dto::ModuleEnum>::AsString& module,
    const oatpp::Object<ModuleActionRequestDto>& body) {

    return processBool(__FUNCTION__, [&](){
        auto m = byUID(systemModule, module, body);
        return wait(m->rebootModuleUsbBootloader());
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::postCanBootloader(
    const oatpp::Enum<dto::ModuleEnum>::AsString& module,
    const oatpp::Object<ModuleActionRequestDto>& body) {

    return processBool(__FUNCTION__, [&](){
        auto m = byUID(systemModule, module, body);
        return wait(m->rebootModuleCanBootloader());
    });
}

// ==========================================
// Core module
// ==========================================

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getShortID() {

    return process(__FUNCTION__, [&](){
        auto sid = wait(systemModule->coreModule()->getShortID());
        auto sidResponseDto = SIDDto::createShared();
        sidResponseDto->sid = sid;
        return createDtoResponse(Status::CODE_200, sidResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getIpAddress() {

    return process(__FUNCTION__, [&](){
        auto ipAddress = wait(systemModule->coreModule()->getIpAddress());
        auto ipResponseDto = IpDto::createShared();
        ipResponseDto->ipAddress = ipAddress;
        return createDtoResponse(Status::CODE_200, ipResponseDto);
    });
}


std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getHostname() {

    return process(__FUNCTION__, [&](){
        auto hostname = wait(systemModule->coreModule()->getHostname());
        auto hostnameResponseDto = HostnameDto::createShared();
        hostnameResponseDto->hostname = hostname;
        return createDtoResponse(Status::CODE_200, hostnameResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getSerialNumber() {

    return process(__FUNCTION__, [&](){
        auto serial = wait(systemModule->coreModule()->getSerialNumber());
        auto serialResponseDto = SerialDto::createShared();
        serialResponseDto->serial = serial;
        return createDtoResponse(Status::CODE_200, serialResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getPowerSupplyType() {

    return process(__FUNCTION__, [&](){
        auto res = wait(systemModule->coreModule()->getPowerSupplyType());
        
        auto supplyTypeResponseDto = SupplyTypeDto::createShared();
        supplyTypeResponseDto->vin = res.isVIN;
        supplyTypeResponseDto->poe = res.isPoE;
        supplyTypeResponseDto->poe_hb = res.isPoE_Hb;
        return createDtoResponse(Status::CODE_200, supplyTypeResponseDto);
    
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getVoltage5V() {

    return process(__FUNCTION__, [&](){
        auto voltage = wait(systemModule->coreModule()->getVoltage5V());
        auto voltageResponseDto = VoltageDto::createShared();
        voltageResponseDto->voltage = voltage;
        return createDtoResponse(Status::CODE_200, voltageResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getVoltageVIN() {

    return process(__FUNCTION__, [&](){
        auto voltage = wait(systemModule->coreModule()->getVoltageVIN());
        auto voltageResponseDto = VoltageDto::createShared();
        voltageResponseDto->voltage = voltage;
        return createDtoResponse(Status::CODE_200, voltageResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getPoEVoltage() {

    return process(__FUNCTION__, [&](){
        auto voltage = wait(systemModule->coreModule()->getVoltagePoE());
        auto voltageResponseDto = VoltageDto::createShared();
        voltageResponseDto->voltage = voltage;
        return createDtoResponse(Status::CODE_200, voltageResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getCurrentConsumption() {

    return process(__FUNCTION__, [&](){
        auto current = wait(systemModule->coreModule()->getCurrentConsumption());
        auto currentResponseDto = CurrentDto::createShared();
        currentResponseDto->current = current;
        return createDtoResponse(Status::CODE_200, currentResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getPowerDraw() {

    return process(__FUNCTION__, [&](){
        auto powerDraw = wait(systemModule->coreModule()->getPowerDraw());
        auto powerDrawResponseDto = PowerDrawDto::createShared();
        powerDrawResponseDto->power_draw = powerDraw;
        return createDtoResponse(Status::CODE_200, powerDrawResponseDto);
    });
}

// ==========================================
// Control module
// ==========================================
std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::setIntensities(const oatpp::Object<IntensitiesDto>& body) {

    return processBool(__FUNCTION__, [&](){
        if (!body || !body->intensity || body->intensity->size() != 4) {
            throw ArgumentException("Invalid intensity array. Must contain exactly 4 values.");
        }

        float ii0, ii1, ii2, ii3;

        auto processIntensity = [](float & i, const oatpp::Float32 & intensity){
            if (!intensity){
                throw ArgumentException("Invalid intensity value. Must be between 0.0 and 1.0.");
            }
            if (*intensity < 0.0f || *intensity > 1.0f) {
                throw ArgumentException("Invalid intensity value. Must be between 0.0 and 1.0.");
            }
            i = *intensity;
        };

        processIntensity(ii0, body->intensity->at(0));
        processIntensity(ii1, body->intensity->at(1));
        processIntensity(ii2, body->intensity->at(2));
        processIntensity(ii3, body->intensity->at(3));
        
        return wait(systemModule->controlModule()->setIntensities(ii0, ii1, ii2, ii3));
    });
/*
    return process(__FUNCTION__, [&](){
        if (!body || !body->intensity || body->intensity->size() != 4) {
            return createResponse(Status::CODE_400, "Invalid intensity array. Must contain exactly 4 values.");
        }

        //add futures into vector
        std::vector < std::shared_future <bool>> futures;

        for (size_t i = 0; i < body->intensity->size(); i++) {
            auto intensity = body->intensity->at(i); 
            if (!intensity){
                continue;
            }
            
            if (*intensity < 0.0f || *intensity > 1.0f) {
                return createResponse(Status::CODE_400, "Invalid intensity value. Must be between 0.0 and 1.0.");
            }

            futures.push_back(systemModule->controlModule()->setIntensity(*intensity, i));
        }

        for (auto f : futures){
            f.wait();
            if (!f.get()){
                 return createResponse(Status::CODE_500, "Cannot set intensity");
            }
        }

        return createResponse(Status::CODE_200, "Intensities set successfully.");
    });*/
}

int SMBRController::getChannel(const dto::ChannelEnum& channel) {
    switch (channel) {
        case dto::ChannelEnum::channel0:
            return 0;
        case dto::ChannelEnum::channel1:
            return 1;
        case dto::ChannelEnum::channel2:
            return 2;
        case dto::ChannelEnum::channel3:
            return 3;
        default:
            throw NotFoundException("Channel not found");
    }
}


std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::setIntensity(
    const oatpp::Enum<dto::ChannelEnum>::AsString& channel, 
    const oatpp::Object<IntensityDto>& body
) {

    return processBool(__FUNCTION__, [&](){
        if (body->intensity < 0 || body->intensity > 1) {
            throw ArgumentException("Invalid intensity. Must be between 0 and 1.");
        }

        return wait(systemModule->controlModule()->setIntensity(body->intensity, getChannel(channel)));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getIntensity(const oatpp::Enum<dto::ChannelEnum>::AsString& channel) {

    return process(__FUNCTION__, [&](){
        auto intensity = wait(systemModule->controlModule()->getIntensity(getChannel(channel)));
        auto intensityResponseDto = IntensityDto::createShared();
        intensityResponseDto->intensity = intensity;
        return createDtoResponse(Status::CODE_200, intensityResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getLedTemperature() {

    return process(__FUNCTION__, [&](){
        auto temperature = wait(systemModule->controlModule()->getLedTemperature());
        auto tempResponseDto = TempDto::createShared();
        tempResponseDto->temperature = temperature;
        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });
}


std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::setHeaterIntensity(const oatpp::Object<IntensityDto>& body) {

    return processBool(__FUNCTION__, [&](){
        if (!body || body->intensity < -1.0f || body->intensity > 1.0f) {
            throw ArgumentException("Invalid intensity value. Must be between -1.0 and 1.0.");
        }

        return wait(systemModule->controlModule()->setHeaterIntensity(body->intensity));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getHeaterIntensity() {

    return process(__FUNCTION__, [&](){
        auto intensity = wait(systemModule->controlModule()->getHeaterIntensity());
        auto intensityResponseDto = IntensityDto::createShared();
        intensityResponseDto->intensity = intensity;
        return createDtoResponse(Status::CODE_200, intensityResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::setHeaterTargetTemperature(const oatpp::Object<TempDto>& body) {
    
    return processBool(__FUNCTION__, [&](){
        if (!body || body->temperature < 0.0f) {
            throw ArgumentException("Invalid target temperature. Must be a positive value.");
        }

        return wait(systemModule->controlModule()->setHeaterTargetTemperature(body->temperature));
    }); 
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getHeaterTargetTemperature() {
    return process(__FUNCTION__, [&](){
        auto temperature = wait(systemModule->controlModule()->getHeaterTargetTemperature());
        
        if (std::isnan(temperature)) {
            auto dto = MessageDto::createShared();
            dto->message = "Target temperature is not set";
            return createDtoResponse(Status::CODE_404, dto); 
        }
        
        auto tempResponseDto = TempDto::createShared();
        tempResponseDto->temperature = oatpp::Float32(temperature);
        
        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getHeaterPlateTemperature() {

    return process(__FUNCTION__, [&](){
        auto plateTemperature = wait(systemModule->controlModule()->getHeaterPlateTemperature());
        auto plateTempResponseDto = TempDto::createShared();
        plateTempResponseDto->temperature = plateTemperature;
        return createDtoResponse(Status::CODE_200, plateTempResponseDto);
    });
}


std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::turnOffHeater() {

    return processBool(__FUNCTION__, [&](){
        return wait(systemModule->controlModule()->turnOffHeater());
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::setCuvettePumpSpeed(const oatpp::Object<SpeedDto>& body) {

    return processBool(__FUNCTION__, [&](){
        if (!body || body->speed < -1.0f || body->speed > 1.0f) {
            throw ArgumentException("Invalid speed value. Must be between -1.0 and 1.0.");
        }

        return wait(systemModule->controlModule()->setCuvettePumpSpeed(body->speed));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getCuvettePumpSpeed() {

    return process(__FUNCTION__, [&](){
        auto speed = wait(systemModule->controlModule()->getCuvettePumpSpeed());
        auto speedResponseDto = SpeedDto::createShared();
        speedResponseDto->speed = speed;
        return createDtoResponse(Status::CODE_200, speedResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::setCuvettePumpFlowrate(const oatpp::Object<FlowrateDto>& body) {

    return processBool(__FUNCTION__, [&](){
        if (!body || body->flowrate < -1000.0f || body->flowrate > 1000.0f) {
            throw ArgumentException("Invalid flowrate value. Must be between -1000.0 and 1000.0.");
        }

        return wait(systemModule->controlModule()->setCuvettePumpFlowrate(body->flowrate));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getCuvettePumpFlowrate() {

    return process(__FUNCTION__, [&](){
        auto flowrate = wait(systemModule->controlModule()->getCuvettePumpFlowrate());
        auto flowrateResponseDto = FlowrateDto::createShared();
        flowrateResponseDto->flowrate = flowrate;
        return createDtoResponse(Status::CODE_200, flowrateResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::moveCuvettePump(const oatpp::Object<MoveDto>& body) {

    return processBool(__FUNCTION__, [&](){
        if (!body || body->volume < 0.0f || body->volume > 1000.0f) {
            throw ArgumentException("Invalid volume value. Must be between 0.0 and 1000.0.");
        }
        if (!body || body->flowrate < -1000.0f || body->flowrate > 1000.0f) {
            throw ArgumentException("Invalid flowrate value. Must be between -1000.0 and 1000.0.");
        }
        return wait(systemModule->controlModule()->moveCuvettePump(body->volume, body->flowrate));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::primeCuvettePump() {

    return processBool(__FUNCTION__, [&](){
        return wait(systemModule->controlModule()->primeCuvettePump());
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::purgeCuvettePump() {

    return processBool(__FUNCTION__, [&](){
        return wait(systemModule->controlModule()->purgeCuvettePump());
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::stopCuvettePump() {

    return processBool(__FUNCTION__, [&](){
        return wait(systemModule->controlModule()->stopCuvettePump());
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::setAeratorSpeed(const oatpp::Object<SpeedDto>& body) {

    return processBool(__FUNCTION__, [&](){
        if (!body || body->speed < 0.0f || body->speed > 1.0f) {
            throw ArgumentException("Invalid speed value. Must be between 0.0 and 1.0.");
        }

        return wait(systemModule->controlModule()->setAeratorSpeed(body->speed));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getAeratorSpeed() {

    return process(__FUNCTION__, [&](){
        auto speed = wait(systemModule->controlModule()->getAeratorSpeed());
        auto speedResponseDto = SpeedDto::createShared();
        speedResponseDto->speed = speed;
        return createDtoResponse(Status::CODE_200, speedResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::setAeratorFlowrate(const oatpp::Object<FlowrateDto>& body) {

    return processBool(__FUNCTION__, [&](){
        if (!body || body->flowrate < 10.0f || body->flowrate > 5000.0f) {
            throw ArgumentException("Invalid flowrate value. Must be between 10.0 and 5000.0 ml/min.");
        }

        return wait(systemModule->controlModule()->setAeratorFlowrate(body->flowrate));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getAeratorFlowrate() {

    return process(__FUNCTION__, [&](){
        auto flowrate = wait(systemModule->controlModule()->getAeratorFlowrate());
        auto flowrateResponseDto = FlowrateDto::createShared();
        flowrateResponseDto->flowrate = flowrate;
        return createDtoResponse(Status::CODE_200, flowrateResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::moveAerator(const oatpp::Object<MoveDto>& body) {
    
    return processBool(__FUNCTION__, [&](){
        if (!body || body->volume < 0.0f || body->volume > 1000.0f) {
            throw ArgumentException("Invalid volume value. Must be between 0.0 and 1000.0.");
        }
        if (!body || body->flowrate < 10.0f || body->flowrate > 5000.0f) {
            throw ArgumentException("Invalid flowrate value. Must be between 10.0 and 5000.0 ml/min.");
        }

        return wait(systemModule->controlModule()->moveAerator(body->volume, body->flowrate));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::stopAerator() {

    return processBool(__FUNCTION__, [&](){
        return wait(systemModule->controlModule()->stopAerator());
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::setMixerSpeed(const oatpp::Object<SpeedDto>& body) {

    return processBool(__FUNCTION__, [&](){
        if (!body || body->speed < 0.0f || body->speed > 1.0f) {
            throw ArgumentException("Invalid speed value. Must be between 0.0 and 1.0.");
        }

        return wait(systemModule->controlModule()->setMixerSpeed(body->speed));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getMixerSpeed() {

    return process(__FUNCTION__, [&](){
        auto speed = wait(systemModule->controlModule()->getMixerSpeed());
        auto speedResponseDto = SpeedDto::createShared();
        speedResponseDto->speed = speed;
        return createDtoResponse(Status::CODE_200, speedResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::setMixerRpm(const oatpp::Object<RpmDto>& body) {

    return processBool(__FUNCTION__, [&](){
        if (!body || body->rpm < 0.0f || body->rpm > 10000.0f) {
            throw ArgumentException("Invalid RPM value. Must be between 0 and 10000.");
        }

        return wait(systemModule->controlModule()->setMixerRpm(body->rpm));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getMixerRpm() {

    return process(__FUNCTION__, [&](){
        auto rpm = wait(systemModule->controlModule()->getMixerRpm());
        auto rpmResponseDto = RpmDto::createShared();
        rpmResponseDto->rpm = rpm;
        return createDtoResponse(Status::CODE_200, rpmResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::stirMixer(const oatpp::Object<StirDto>& body) {

    return processBool(__FUNCTION__, [&](){
        if (!body || body->rpm < 0.0f || body->rpm > 10000.0f || body->time < 0.0f || body->time > 3600.0f) {
            throw ArgumentException("Invalid RPM or time value. RPM must be between 0 and 10000, and time must be between 0 and 3600 seconds.");
        }

        return wait(systemModule->controlModule()->stirMixer(body->rpm, body->time));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::stopMixer() {

    return processBool(__FUNCTION__, [&](){
        return wait(systemModule->controlModule()->stopMixer());
    });
}

// ==========================================
// Sensor module
// ==========================================

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getBottleTemperature() {

    return process(__FUNCTION__, [&](){
        auto temperature = wait(systemModule->sensorModule()->getBottleTemperature());
        auto tempResponseDto = TempDto::createShared();
        tempResponseDto->temperature = temperature;
        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getTopMeasuredTemperature() {

    return process(__FUNCTION__, [&](){
        auto temperature = wait(systemModule->sensorModule()->getTopMeasuredTemperature());
        auto tempResponseDto = TempDto::createShared();
        tempResponseDto->temperature = temperature;
        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getBottomMeasuredTemperature() {

    return process(__FUNCTION__, [&](){
        auto temperature = wait(systemModule->sensorModule()->getBottomMeasuredTemperature());
        auto tempResponseDto = TempDto::createShared();
        tempResponseDto->temperature = temperature;
        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getTopSensorTemperature() {

    return process(__FUNCTION__, [&](){
        auto temperature = wait(systemModule->sensorModule()->getTopSensorTemperature());
        auto tempResponseDto = TempDto::createShared();
        tempResponseDto->temperature = temperature;
        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getBottomSensorTemperature() {

    return process(__FUNCTION__, [&](){
        auto temperature = wait(systemModule->sensorModule()->getBottomSensorTemperature());
        auto tempResponseDto = TempDto::createShared();
        tempResponseDto->temperature = temperature;
        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::clearCustomText() {
    return processBool(__FUNCTION__, [&](){
        return wait(systemModule->sensorModule()->clearCustomText());
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::printCustomText(const oatpp::Object<TextDto>& body) {
    return processBool(__FUNCTION__, [&](){
        return wait(systemModule->sensorModule()->printCustomText(body->text));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::checkFluorometerOjipCaptureComplete() {
    return process(__FUNCTION__, [&](){
        auto captureComplete = wait(systemModule->sensorModule()->isFluorometerOjipCaptureComplete());
        auto captureStatusDto = FluorometerCaptureStatusDto::createShared();
        captureStatusDto->capture_complete = captureComplete;
        return createDtoResponse(Status::CODE_200, captureStatusDto);
    });
}

// ==========================================
// Recipes
// ==========================================

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getRecipeList() {
    try {
        auto sc = recipes_->getRecipeNames();
        auto dtoList = oatpp::List<oatpp::String>::createShared();
        for (auto s : sc){
            dtoList->push_back(s);
        }
        return createDtoResponse(Status::CODE_200, dtoList);
    } catch (std::exception & e){
        return createResponse(Status::CODE_500, "Failed to retrieve recipe list: " + std::string(e.what()));
    }
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::reloadRecipeList() {
    try {
        recipes_->reload();
        return getRecipeList();
    } catch (std::exception & e){
        return createResponse(Status::CODE_500, "Failed to reload recipe: " + std::string(e.what()));
    }
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getRecipeContent(const oatpp::String& name) {
    try {
        auto s = recipes_->getRecipeContent(name);
        auto scriptResponseDto = ScriptDto::createShared();
        scriptResponseDto->name = s.name;
        scriptResponseDto->content = s.content;
        return createDtoResponse(Status::CODE_200, scriptResponseDto);
    } catch (std::exception & e){
        return createResponse(Status::CODE_500, "Failed to retrieve recipe: " + std::string(e.what()));
    }
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::updateRecipe(const oatpp::String& name, const oatpp::Object<ScriptDto>& body) {
    try {
        if (!body || !body->name || !body->content) {
            throw ArgumentException("Invalid script. Must contain a name and content.");
        }
        ScriptInfo s;
        s.name = *body->name;
        s.content = *body->content;
        recipes_->replaceRecipe(s);
        return createResponse(Status::CODE_200, "Recipe updated successfully.");
    } catch (std::exception & e){
        return createResponse(Status::CODE_500, "Failed to update recipe: " + std::string(e.what()));
    }
}    

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::deleteRecipe(const oatpp::String& name) {
    try {
        recipes_->deleteRecipe(name);
        return createResponse(Status::CODE_200, "Recipe deleted successfully.");
    } catch (std::exception & e){
        return createResponse(Status::CODE_500, "Failed to delete recipe: " + std::string(e.what()));
    }
}
    

// ==========================================
// Scheduler
// ==========================================
/*
std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::uploadScript(const oatpp::Object<ScriptDto>& body) {
    
    try {
        if (!body || !body->name || !body->content) {
            throw ArgumentException("Invalid script. Must contain a name and content.");
        }
        ScriptInfo s;
        s.name = *body->name;
        s.content = *body->content;
        scheduler_->setScriptFromString(s);
        return createResponse(Status::CODE_200, "Script uploaded successfully.");
    } catch (std::exception & e){
        return createResponse(Status::CODE_500, "Failed to upload script: " + std::string(e.what()));
    }
}*/


std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::selectRecipe(const oatpp::String& name) {
    try {
        auto sc = recipes_->getRecipeContent(name);
        scheduler_->setScriptFromString(sc);
        return createResponse(Status::CODE_200, "Recipe " + name + " selected.");
    } catch (std::exception & e){
        return createResponse(Status::CODE_500, "Failed to select script: " + std::string(e.what()));
    }
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getRecipe() {
    try {
        auto s = scheduler_->getScript();
        auto scriptResponseDto = ScriptDto::createShared();
        scriptResponseDto->name = s.name;
        scriptResponseDto->content = s.content;
        return createDtoResponse(Status::CODE_200, scriptResponseDto);
    } catch (std::exception & e){
        return createResponse(Status::CODE_500, "Failed to retrieve recipe: " + std::string(e.what()));
    }
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::startScheduler() {
    try {
        std::cout << "start called from API" << std::endl;
        auto processId = scheduler_->start();
        auto scriptProcessIdDto = ScriptProcessIdDto::createShared();
        scriptProcessIdDto->processId = processId;
        return createDtoResponse(Status::CODE_200, scriptProcessIdDto);
    } catch (std::exception & e){
        return createResponse(Status::CODE_500, "Failed to start script: " + std::string(e.what()));
    }
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::stopScheduler() {
    try {
        std::cout << "stop called from API" << std::endl;
        scheduler_->stop();
        return createResponse(Status::CODE_200, "Script stopped successfully.");
    } catch (std::exception & e){
        return createResponse(Status::CODE_500, "Failed to stop script: " + std::string(e.what()));
    }
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getSchedulerInfo() {
    try {
        RuntimeInfo info = scheduler_->getRuntimeInfo();
        auto infoResponseDto = ScriptRuntimeInfoDto::createShared();

        infoResponseDto->processId = info.processId;
        infoResponseDto->name = info.name;
        infoResponseDto->finalMessage = info.finishMessage;
        infoResponseDto->stack = oatpp::Vector<Int32>::createShared();
        for (auto i : info.stack){
            infoResponseDto->stack->push_back(i);
        }
        infoResponseDto->output = oatpp::Vector<String>::createShared();
        for (auto o : info.output){
            std::stringstream s;
            s << o;
            infoResponseDto->output->push_back(s.str());
        }
        infoResponseDto->started = info.started;
        infoResponseDto->stopped = info.stopped;

        infoResponseDto->startedAt = Poco::DateTimeFormatter::format(info.startTime, "%Y-%m-%d %H:%M:%S");;
        
        return createDtoResponse(Status::CODE_200, infoResponseDto);

    } catch (std::exception & e){
        return createResponse(Status::CODE_500, "Failed to retrieve scheduler status: " + std::string(e.what()));
    }
}

