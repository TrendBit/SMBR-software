#include "ControlController.hpp"

#include "SMBR/Exceptions.hpp"
#include "SMBR/Log.hpp"
#include "ControllerUtils.hpp"

#include <cmath>

#include <chrono>

using namespace std::chrono_literals;

ControlController::ControlController(const std::shared_ptr<oatpp::web::mime::ContentMappers>& apiContentMappers,
                                     std::shared_ptr<ISystemModule> systemModule)
    : SMBRControllerBase(apiContentMappers, systemModule)
{}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ControlController::setIntensities(const oatpp::Object<IntensitiesDto>& body) {
    return processBool(__FUNCTION__, [&](){
        if (!body->intensity || body->intensity->size() != 4) {
            throw ArgumentException("intensity array must contain exactly 4 values");
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

        return waitFor(systemModule->controlModule()->setIntensities(ii0, ii1, ii2, ii3));
    });
}

int ControlController::getChannel(const dto::ChannelEnum& channel) {
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


std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ControlController::setIntensity(const oatpp::Enum<dto::ChannelEnum>::AsString& channel,
    const oatpp::Object<IntensityDto>& body
) {

    return processBool(__FUNCTION__, [&](){
        if (!body) {
            throw ArgumentException("Request body is required.");
        }
        
        if (!body->intensity) {
            throw ArgumentException("Intensity field is required.");
        }
        
        float intensityValue = body->intensity;
        if (intensityValue < 0 || intensityValue > 1) {
            throw ArgumentException("Invalid intensity. Must be between 0 and 1.");
        }

        return waitFor(systemModule->controlModule()->setIntensity(intensityValue, getChannel(channel)));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ControlController::getIntensity(const oatpp::Enum<dto::ChannelEnum>::AsString& channel) {

    return process(__FUNCTION__, [&](){
        auto response = waitFor(systemModule->controlModule()->getIntensity(getChannel(channel)));
        auto intensityResponseDto = IntensityDto::createShared();
        intensityResponseDto->intensity = response.intensity;
        return createDtoResponse(Status::CODE_200, intensityResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ControlController::getLedTemperature() {

    return process(__FUNCTION__, [&](){
        auto temperature = waitFor(systemModule->controlModule()->getLedTemperature());
        auto tempResponseDto = TempDto::createShared();
        tempResponseDto->temperature = temperature;
        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });
}


std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ControlController::setHeaterIntensity(const oatpp::Object<IntensityDto>& body) {

    return processBool(__FUNCTION__, [&](){
        if (!body) {
            throw ArgumentException("Request body is required.");
        }
        
        if (!body->intensity) {
            throw ArgumentException("Intensity field is required.");
        }
        
        float intensityValue = body->intensity;
        if (intensityValue < -1.0 || intensityValue > 1.0) {
            throw ArgumentException("Invalid intensity. Must be between -1.0 and 1.0.");
        }

        return waitFor(systemModule->controlModule()->setHeaterIntensity(intensityValue));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ControlController::getHeaterIntensity() {

    return process(__FUNCTION__, [&](){
        auto intensity = waitFor(systemModule->controlModule()->getHeaterIntensity());
        auto intensityResponseDto = IntensityDto::createShared();
        intensityResponseDto->intensity = intensity;
        return createDtoResponse(Status::CODE_200, intensityResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ControlController::setHeaterTargetTemperature(const oatpp::Object<TempDto>& body) {

    return processBool(__FUNCTION__, [&](){
        if (!body) {
            throw ArgumentException("Request body is required.");
        }
        
        if (!body->temperature) {
            throw ArgumentException("Temperature field is required.");
        }
        
        float temperatureValue = body->temperature;
        if (temperatureValue < 0.0f || temperatureValue > 60.0f) {
            throw ArgumentException("Invalid target temperature. Must be between 0 and 60.");
        }

        return waitFor(systemModule->controlModule()->setHeaterTargetTemperature(temperatureValue));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ControlController::getHeaterTargetTemperature() {
    return process(__FUNCTION__, [&](){
        auto temperature = waitFor(systemModule->controlModule()->getHeaterTargetTemperature());

        auto tempResponseDto = TempDto::createShared();

        if (std::isnan(temperature)) {
            auto tempNullDto = TempNullDto::createShared();
            tempNullDto->temperature = nullptr;
            return createDtoResponse(Status::CODE_200, tempNullDto);
        }

        tempResponseDto->temperature = oatpp::Float32(temperature);

        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ControlController::getHeaterPlateTemperature() {

    return process(__FUNCTION__, [&](){
        auto plateTemperature = waitFor(systemModule->controlModule()->getHeaterPlateTemperature());
        auto plateTempResponseDto = TempDto::createShared();
        plateTempResponseDto->temperature = plateTemperature;
        return createDtoResponse(Status::CODE_200, plateTempResponseDto);
    });
}


std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ControlController::turnOffHeater() {

    return processBool(__FUNCTION__, [&](){
        return waitFor(systemModule->controlModule()->turnOffHeater());
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ControlController::getCuvettePumpInfo() {
    return process(__FUNCTION__, [&]() {
        auto response = waitFor(systemModule->controlModule()->getCuvettePumpInfo());
        auto dto = CuvettePumpInfoDto::createShared();
        dto->max_flowrate = response.max;
        dto->min_flowrate = response.min;
        return createDtoResponse(Status::CODE_200, dto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ControlController::setCuvettePumpSpeed(const oatpp::Object<SpeedDto>& body) {

    return processBool(__FUNCTION__, [&](){
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

        return waitFor(systemModule->controlModule()->setCuvettePumpSpeed(speedValue));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ControlController::getCuvettePumpSpeed() {

    return process(__FUNCTION__, [&](){
        auto speed = waitFor(systemModule->controlModule()->getCuvettePumpSpeed());
        auto speedResponseDto = SpeedDto::createShared();
        speedResponseDto->speed = speed;
        return createDtoResponse(Status::CODE_200, speedResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ControlController::setCuvettePumpFlowrate(const oatpp::Object<FlowrateDto>& body) {

    return processBool(__FUNCTION__, [&](){
        if (!body) {
            throw ArgumentException("Request body is required.");
        }
        
        if (!body->flowrate) {
            throw ArgumentException("Flowrate field is required.");
        }
        
        float flowrateValue = body->flowrate;
        if (flowrateValue < -1000.0f || flowrateValue > 1000.0f) {
            throw ArgumentException("Invalid flowrate value. Must be between -1000.0 and 1000.0.");
        }

        return waitFor(systemModule->controlModule()->setCuvettePumpFlowrate(flowrateValue));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ControlController::getCuvettePumpFlowrate() {

    return process(__FUNCTION__, [&](){
        auto flowrate = waitFor(systemModule->controlModule()->getCuvettePumpFlowrate());
        auto flowrateResponseDto = FlowrateDto::createShared();
        flowrateResponseDto->flowrate = flowrate;
        return createDtoResponse(Status::CODE_200, flowrateResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ControlController::moveCuvettePump(const oatpp::Object<MoveDto>& body) {

    return processBool(__FUNCTION__, [&](){
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
        if (volumeValue < 0.0f || volumeValue > 1000.0f) {
            throw ArgumentException("Invalid volume value. Must be between 0.0 and 1000.0.");
        }
        
        float flowrateValue = body->flowrate;
        if (flowrateValue < -1000.0f || flowrateValue > 1000.0f) {
            throw ArgumentException("Invalid flowrate value. Must be between -1000.0 and 1000.0.");
        }

        return waitFor(systemModule->controlModule()->moveCuvettePump(volumeValue, flowrateValue));
    });
}
/*
std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ControlController::primeCuvettePump() {

    return processBool(__FUNCTION__, [&](){
        return waitFor(systemModule->controlModule()->primeCuvettePump());
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ControlController::purgeCuvettePump() {

    return processBool(__FUNCTION__, [&](){
        return waitFor(systemModule->controlModule()->purgeCuvettePump());
    });
}*/

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ControlController::stopCuvettePump() {

    return processBool(__FUNCTION__, [&](){
        return waitFor(systemModule->controlModule()->stopCuvettePump());
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ControlController::calibrateCuvettePump(const oatpp::Object<FlowrateDto>& body) {

    return processBool(__FUNCTION__, [&](){
        if (!body) {
            throw ArgumentException("Request body is required.");
        }
        
        if (!body->flowrate) {
            throw ArgumentException("Flowrate field is required.");
        }
        
        float flowrateValue = body->flowrate;
        if (flowrateValue < 0.0f || flowrateValue > 1000.0f) {
            throw ArgumentException("Invalid flowrate value. Must be between 0.0 and 1000.0.");
        }

        return waitFor(systemModule->controlModule()->setCuvettePumpMaxFlowrate(flowrateValue));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ControlController::getAeratorInfo() {
    return process(__FUNCTION__, [&](){
        auto result = waitFor(systemModule->controlModule()->getAeratorInfo());
        auto dto = AeratorInfoDto::createShared();
        dto->max_flowrate = result.max;
        dto->min_flowrate = result.min;
        return createDtoResponse(Status::CODE_200, dto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ControlController::setAeratorSpeed(const oatpp::Object<SpeedDto>& body) {

    return processBool(__FUNCTION__, [&](){
        if (!body) {
            throw ArgumentException("Request body is required.");
        }
        
        if (!body->speed) {
            throw ArgumentException("Speed field is required.");
        }
        
        float speedValue = body->speed;
        if (speedValue < 0.0f || speedValue > 1.0f) {
            throw ArgumentException("Invalid speed value. Must be between 0.0 and 1.0.");
        }

        return waitFor(systemModule->controlModule()->setAeratorSpeed(speedValue));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ControlController::getAeratorSpeed() {

    return process(__FUNCTION__, [&](){
        auto speed = waitFor(systemModule->controlModule()->getAeratorSpeed());
        auto speedResponseDto = SpeedDto::createShared();
        speedResponseDto->speed = speed;
        return createDtoResponse(Status::CODE_200, speedResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ControlController::setAeratorFlowrate(const oatpp::Object<FlowrateDto>& body) {

    return processBool(__FUNCTION__, [&](){
        if (!body) {
            throw ArgumentException("Request body is required.");
        }
        
        if (!body->flowrate) {
            throw ArgumentException("Flowrate field is required.");
        }
        
        float flowrateValue = body->flowrate;
        if (flowrateValue < 10.0f || flowrateValue > 5000.0f) {
            throw ArgumentException("Invalid flowrate value. Must be between 10.0 and 5000.0 ml/min.");
        }

        return waitFor(systemModule->controlModule()->setAeratorFlowrate(flowrateValue));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ControlController::getAeratorFlowrate() {

    return process(__FUNCTION__, [&](){
        auto flowrate = waitFor(systemModule->controlModule()->getAeratorFlowrate());
        auto flowrateResponseDto = FlowrateDto::createShared();
        flowrateResponseDto->flowrate = flowrate;
        return createDtoResponse(Status::CODE_200, flowrateResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ControlController::calibrateAerator(const oatpp::Object<FlowrateDto>& body) {

    return processBool(__FUNCTION__, [&](){
        if (!body) {
            throw ArgumentException("Request body is required.");
        }

        if (!body->flowrate) {
            throw ArgumentException("Flowrate field is required.");
        }

        float flowrateValue = body->flowrate;
        if (flowrateValue < 0.0f || flowrateValue > 1000.0f) {
            throw ArgumentException("Invalid flowrate value. Must be between 0.0 and 1000.0.");
        }

        return waitFor(systemModule->controlModule()->setAeratorMaxFlowrate(flowrateValue));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ControlController::moveAerator(const oatpp::Object<MoveDto>& body) {

    return processBool(__FUNCTION__, [&](){
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
        if (volumeValue < 0.0f || volumeValue > 1000.0f) {
            throw ArgumentException("Invalid volume value. Must be between 0.0 and 1000.0.");
        }
        
        float flowrateValue = body->flowrate;
        if (flowrateValue < 10.0f || flowrateValue > 5000.0f) {
            throw ArgumentException("Invalid flowrate value. Must be between 10.0 and 5000.0 ml/min.");
        }

        return waitFor(systemModule->controlModule()->moveAerator(volumeValue, flowrateValue));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ControlController::stopAerator() {

    return processBool(__FUNCTION__, [&](){
        return waitFor(systemModule->controlModule()->stopAerator());
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ControlController::getMixerInfo() {
    return process(__FUNCTION__, [&](){
        auto result = waitFor(systemModule->controlModule()->getMixerInfo());
        auto dto = MixerInfoDto::createShared();
        dto->max_rpm = result.maxRPM;
        dto->min_rpm = result.minRPM;
        return createDtoResponse(Status::CODE_200, dto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ControlController::setMixerSpeed(const oatpp::Object<SpeedDto>& body) {

    return processBool(__FUNCTION__, [&](){
        if (!body) {
            throw ArgumentException("Request body is required.");
        }
        
        if (!body->speed) {
            throw ArgumentException("Speed field is required.");
        }
        
        float speedValue = body->speed;
        if (speedValue < 0.0f || speedValue > 1.0f) {
            throw ArgumentException("Invalid speed value. Must be between 0.0 and 1.0.");
        }

        return waitFor(systemModule->controlModule()->setMixerSpeed(speedValue));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ControlController::getMixerSpeed() {

    return process(__FUNCTION__, [&](){
        auto speed = waitFor(systemModule->controlModule()->getMixerSpeed());
        auto speedResponseDto = SpeedDto::createShared();
        speedResponseDto->speed = speed;
        return createDtoResponse(Status::CODE_200, speedResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ControlController::setMixerRpm(const oatpp::Object<RpmDto>& body) {

    return processBool(__FUNCTION__, [&](){
        if (!body) {
            throw ArgumentException("Request body is required.");
        }
        
        if (!body->rpm) {
            throw ArgumentException("RPM field is required.");
        }
        
        float rpmValue = body->rpm;
        if (rpmValue < 0.0f || rpmValue > 10000.0f) {
            throw ArgumentException("Invalid RPM value. Must be between 0 and 10000.");
        }

        return waitFor(systemModule->controlModule()->setMixerRpm(rpmValue));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ControlController::getMixerRpm() {

    return process(__FUNCTION__, [&](){
        auto rpm = waitFor(systemModule->controlModule()->getMixerRpm());
        auto rpmResponseDto = RpmDto::createShared();
        rpmResponseDto->rpm = rpm;
        return createDtoResponse(Status::CODE_200, rpmResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ControlController::stirMixer(const oatpp::Object<StirDto>& body) {

    return processBool(__FUNCTION__, [&](){
        if (!body) {
            throw ArgumentException("Request body is required.");
        }
        
        if (!body->rpm) {
            throw ArgumentException("RPM field is required.");
        }
        
        if (!body->time) {
            throw ArgumentException("Time field is required.");
        }
        
        float rpmValue = body->rpm;
        if (rpmValue < 0.0f || rpmValue > 10000.0f) {
            throw ArgumentException("Invalid RPM value. RPM must be between 0 and 10000.");
        }
        
        float timeValue = body->time;
        if (timeValue < 0.0f || timeValue > 3600.0f) {
            throw ArgumentException("Invalid time value. Must be between 0 and 3600 seconds.");
        }

        return waitFor(systemModule->controlModule()->stirMixer(rpmValue, timeValue));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ControlController::stopMixer() {

    return processBool(__FUNCTION__, [&](){
        return waitFor(systemModule->controlModule()->stopMixer());
    });
}
