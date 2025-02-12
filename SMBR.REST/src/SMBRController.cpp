#include "SMBRController.hpp"
#include "SMBR/Exceptions.hpp"


std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::process(
    std::string name,
    std::function<std::shared_ptr<oatpp::web::protocol::http::outgoing::Response>()> body
){
    try {
        return body();
    } catch (TimeoutException & e){
        return createResponse(Status::CODE_504, name + " timed out");
    } catch (NotFoundException & e){
        return createResponse(Status::CODE_404, name + " not found");
    } catch (std::exception & e){
        return createResponse(Status::CODE_500, "Failed to retrieve " + name);
    }
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::processBool(
    std::string name,
    std::function <bool ()> body
){
    try {
        bool success = body();
        if (success) {
            return createResponse(Status::CODE_200, name + " successful");
        } else {
            return createResponse(Status::CODE_500, name + " failed");
        }
        
    } catch (TimeoutException & e){
        return createResponse(Status::CODE_504, name + " timed out");
    } catch (NotFoundException & e){
        return createResponse(Status::CODE_404, name + " not found");
    } catch (std::exception & e){
        return createResponse(Status::CODE_500, "Failed to retrieve " + name);
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

    }

  // ==========================================
  // System Endpoints
  // ==========================================

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getSystemModules() {
    auto dtoList = oatpp::List<oatpp::Object<MyModuleInfoDto>>::createShared();

    auto future = systemModule->getAvailableModules();        

    future.wait();
    auto result = future.get();

    for (auto m : result){
        auto moduleInfoDto = MyModuleInfoDto::createShared();
        
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
        dtoList->push_back(moduleInfoDto);
    }

    return createDtoResponse(Status::CODE_200, dtoList);
}

  // ==========================================
  // Common Endpoints
  // ==========================================
/*
uint8_t SMBRController::getNextSeqNumber() {
    uint8_t currentSeq = m_seqNum.fetch_add(1);
    if (currentSeq == 255) {
        m_seqNum = 0;  
    }
    return currentSeq;
}*/

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
        auto pingResponseDto = MyPingResponseDto::createShared();
        pingResponseDto->time_ms = responseTime;
        return createDtoResponse(Status::CODE_200, pingResponseDto);      
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getCoreLoad(const oatpp::Enum<dto::ModuleEnum>::AsString& module) {
    return process(__FUNCTION__, [&](){
        float load = wait(getModule(module)->getCoreLoad()); 
        auto loadResponseDto = MyLoadResponseDto::createShared();
        loadResponseDto->load = load;
        return createDtoResponse(Status::CODE_200, loadResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getCoreTemp(const oatpp::Enum<dto::ModuleEnum>::AsString& module) {
    return process(__FUNCTION__, [&](){
        float temperature = wait(getModule(module)->getCoreTemp());
        auto tempResponseDto = MyTempDto::createShared();
        tempResponseDto->temperature = temperature;
        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getBoardTemp(const oatpp::Enum<dto::ModuleEnum>::AsString& module) {
    return process(__FUNCTION__, [&](){
        float temperature = wait(getModule(module)->getBoardTemp());
        auto tempResponseDto = MyTempDto::createShared();
        tempResponseDto->temperature = temperature;
        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });
}

static ICommonModule::Ptr byUID(ISystemModule::Ptr s, const oatpp::data::type::EnumObjectWrapper<dto::ModuleEnum, oatpp::data::type::EnumInterpreterAsString<dto::ModuleEnum, false>>& module,
    const oatpp::Object<MyModuleActionRequestDto>& body){
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
    const oatpp::Object<MyModuleActionRequestDto>& body) {

    return processBool(__FUNCTION__, [&](){
        auto m = byUID(systemModule, module, body);
        return wait(m->restartModule());
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::postUsbBootloader(
    const oatpp::Enum<dto::ModuleEnum>::AsString& module,
    const oatpp::Object<MyModuleActionRequestDto>& body) {

    return processBool(__FUNCTION__, [&](){
        auto m = byUID(systemModule, module, body);
        return wait(m->rebootModuleUsbBootloader());
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::postCanBootloader(
    const oatpp::Enum<dto::ModuleEnum>::AsString& module,
    const oatpp::Object<MyModuleActionRequestDto>& body) {

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
        auto sidResponseDto = MySIDDto::createShared();
        sidResponseDto->sid = sid;
        return createDtoResponse(Status::CODE_200, sidResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getIpAddress() {

    return process(__FUNCTION__, [&](){
        auto ipAddress = wait(systemModule->coreModule()->getIpAddress());
        auto ipResponseDto = MyIpDto::createShared();
        ipResponseDto->ipAddress = ipAddress;
        return createDtoResponse(Status::CODE_200, ipResponseDto);
    });

}


std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getHostname() {

    return process(__FUNCTION__, [&](){
        auto hostname = wait(systemModule->coreModule()->getHostname());
        auto hostnameResponseDto = MyHostnameDto::createShared();
        hostnameResponseDto->hostname = hostname;
        return createDtoResponse(Status::CODE_200, hostnameResponseDto);
    });

}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getSerialNumber() {

    return process(__FUNCTION__, [&](){
        auto serial = wait(systemModule->coreModule()->getSerialNumber());
        auto serialResponseDto = MySerialDto::createShared();
        serialResponseDto->serial = serial;
        return createDtoResponse(Status::CODE_200, serialResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getPowerSupplyType() {

    return process(__FUNCTION__, [&](){
        auto res = wait(systemModule->coreModule()->getPowerSupplyType());
        
        auto supplyTypeResponseDto = MySupplyTypeDto::createShared();
        supplyTypeResponseDto->vin = res.isVIN;
        supplyTypeResponseDto->poe = res.isPoE;
        supplyTypeResponseDto->poe_hb = res.isPoE_Hb;
        return createDtoResponse(Status::CODE_200, supplyTypeResponseDto);
    
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getVoltage5V() {

    return process(__FUNCTION__, [&](){
        auto voltage = wait(systemModule->coreModule()->getVoltage5V());
        auto voltageResponseDto = MyVoltageDto::createShared();
        voltageResponseDto->voltage = voltage;
        return createDtoResponse(Status::CODE_200, voltageResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getVoltageVIN() {

    return process(__FUNCTION__, [&](){
        auto voltage = wait(systemModule->coreModule()->getVoltageVIN());
        auto voltageResponseDto = MyVoltageDto::createShared();
        voltageResponseDto->voltage = voltage;
        return createDtoResponse(Status::CODE_200, voltageResponseDto);
    });

}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getPoEVoltage() {

    return process(__FUNCTION__, [&](){
        auto voltage = wait(systemModule->coreModule()->getVoltagePoE());
        auto voltageResponseDto = MyVoltageDto::createShared();
        voltageResponseDto->voltage = voltage;
        return createDtoResponse(Status::CODE_200, voltageResponseDto);
    });

}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getCurrentConsumption() {

    return process(__FUNCTION__, [&](){
        auto current = wait(systemModule->coreModule()->getCurrentConsumption());
        auto currentResponseDto = MyCurrentDto::createShared();
        currentResponseDto->current = current;
        return createDtoResponse(Status::CODE_200, currentResponseDto);
    });

}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getPowerDraw() {

    return process(__FUNCTION__, [&](){
        auto powerDraw = wait(systemModule->coreModule()->getPowerDraw());
        auto powerDrawResponseDto = MyPowerDrawDto::createShared();
        powerDrawResponseDto->power_draw = powerDraw;
        return createDtoResponse(Status::CODE_200, powerDrawResponseDto);
    });

}

// ==========================================
// Control module
// ==========================================
std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::setIntensities(const oatpp::Object<MyIntensitiesDto>& body) {

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
    const oatpp::Object<MyIntensityDto>& body
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
        auto intensityResponseDto = MyIntensityDto::createShared();
        intensityResponseDto->intensity = intensity;
        return createDtoResponse(Status::CODE_200, intensityResponseDto);
    });

}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getLedTemperature() {

    return process(__FUNCTION__, [&](){
        auto temperature = wait(systemModule->controlModule()->getLedTemperature());
        auto tempResponseDto = MyTempDto::createShared();
        tempResponseDto->temperature = temperature;
        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });

}


std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::setHeaterIntensity(const oatpp::Object<MyIntensityDto>& body) {

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
        auto intensityResponseDto = MyIntensityDto::createShared();
        intensityResponseDto->intensity = intensity;
        return createDtoResponse(Status::CODE_200, intensityResponseDto);
    });

}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::setHeaterTargetTemperature(const oatpp::Object<MyTempDto>& body) {
    
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
        auto tempResponseDto = MyTempDto::createShared();
        tempResponseDto->temperature = temperature;
        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });

}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getHeaterPlateTemperature() {

    return process(__FUNCTION__, [&](){
        auto plateTemperature = wait(systemModule->controlModule()->getHeaterPlateTemperature());
        auto plateTempResponseDto = MyTempDto::createShared();
        plateTempResponseDto->temperature = plateTemperature;
        return createDtoResponse(Status::CODE_200, plateTempResponseDto);
    });

}


std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::turnOffHeater() {

    return processBool(__FUNCTION__, [&](){
        return wait(systemModule->controlModule()->turnOffHeater());
    });

}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::setCuvettePumpSpeed(const oatpp::Object<MySpeedDto>& body) {

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
        auto speedResponseDto = MySpeedDto::createShared();
        speedResponseDto->speed = speed;
        return createDtoResponse(Status::CODE_200, speedResponseDto);
    });

}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::setCuvettePumpFlowrate(const oatpp::Object<MyFlowrateDto>& body) {

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
        auto flowrateResponseDto = MyFlowrateDto::createShared();
        flowrateResponseDto->flowrate = flowrate;
        return createDtoResponse(Status::CODE_200, flowrateResponseDto);
    });

}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::moveCuvettePump(const oatpp::Object<MyMoveDto>& body) {

    return process(__FUNCTION__, [&](){
        if (!body || body->volume < 0.0f || body->volume > 1000.0f) {
            throw ArgumentException("Invalid volume value. Must be between 0.0 and 1000.0.");
        }
        if (!body || body->flowrate < -1000.0f || body->flowrate > 1000.0f) {
            throw ArgumentException("Invalid flowrate value. Must be between -1000.0 and 1000.0.");
        }

        bool success = wait(systemModule->controlModule()->moveCuvettePump(body->volume, body->flowrate));
        if (success) {
            return createResponse(Status::CODE_200, "Successfully started moving liquid.");
        } else {
            return createResponse(Status::CODE_500, "Failed to start moving liquid.");
        }
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

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::setAeratorSpeed(const oatpp::Object<MySpeedDto>& body) {

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
        auto speedResponseDto = MySpeedDto::createShared();
        speedResponseDto->speed = speed;
        return createDtoResponse(Status::CODE_200, speedResponseDto);
    });

}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::setAeratorFlowrate(const oatpp::Object<MyFlowrateDto>& body) {

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
        auto flowrateResponseDto = MyFlowrateDto::createShared();
        flowrateResponseDto->flowrate = flowrate;
        return createDtoResponse(Status::CODE_200, flowrateResponseDto);
    });

}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::moveAerator(const oatpp::Object<MyMoveDto>& body) {

    return process(__FUNCTION__, [&](){
        if (!body || body->volume < 0.0f || body->volume > 1000.0f) {
            throw ArgumentException("Invalid volume value. Must be between 0.0 and 1000.0.");
        }
        if (!body || body->flowrate < 10.0f || body->flowrate > 5000.0f) {
            throw ArgumentException("Invalid flowrate value. Must be between 10.0 and 5000.0 ml/min.");
        }

        bool success = wait(systemModule->controlModule()->moveAerator(body->volume, body->flowrate));
        if (success) {
            auto moveResponseDto = MyMoveDto::createShared();
            moveResponseDto->volume = body->volume;
            moveResponseDto->flowrate = body->flowrate;
            return createDtoResponse(Status::CODE_200, moveResponseDto);
        } else {
            return createResponse(Status::CODE_500, "Failed to start moving air.");
        }
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::stopAerator() {

    return processBool(__FUNCTION__, [&](){
        return wait(systemModule->controlModule()->stopAerator());
    });

}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::setMixerSpeed(const oatpp::Object<MySpeedDto>& body) {

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
        auto speedResponseDto = MySpeedDto::createShared();
        speedResponseDto->speed = speed;
        return createDtoResponse(Status::CODE_200, speedResponseDto);
    });

}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::setMixerRpm(const oatpp::Object<MyRpmDto>& body) {

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
        auto rpmResponseDto = MyRpmDto::createShared();
        rpmResponseDto->rpm = rpm;
        return createDtoResponse(Status::CODE_200, rpmResponseDto);
    });

}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::stirMixer(const oatpp::Object<MyStirDto>& body) {

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
        auto tempResponseDto = MyTempDto::createShared();
        tempResponseDto->temperature = temperature;
        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });

}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getTopMeasuredTemperature() {

    return process(__FUNCTION__, [&](){
        auto temperature = wait(systemModule->sensorModule()->getTopMeasuredTemperature());
        auto tempResponseDto = MyTempDto::createShared();
        tempResponseDto->temperature = temperature;
        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });

}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getBottomMeasuredTemperature() {

    return process(__FUNCTION__, [&](){
        auto temperature = wait(systemModule->sensorModule()->getBottomMeasuredTemperature());
        auto tempResponseDto = MyTempDto::createShared();
        tempResponseDto->temperature = temperature;
        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });

}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getTopSensorTemperature() {

    return process(__FUNCTION__, [&](){
        auto temperature = wait(systemModule->sensorModule()->getTopSensorTemperature());
        auto tempResponseDto = MyTempDto::createShared();
        tempResponseDto->temperature = temperature;
        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });

}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getBottomSensorTemperature() {

    return process(__FUNCTION__, [&](){
        auto temperature = wait(systemModule->sensorModule()->getBottomSensorTemperature());
        auto tempResponseDto = MyTempDto::createShared();
        tempResponseDto->temperature = temperature;
        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });

}
