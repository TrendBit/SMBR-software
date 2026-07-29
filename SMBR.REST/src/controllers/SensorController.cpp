#include "SensorController.hpp"

#include "SMBR/Exceptions.hpp"
#include "SMBR/Log.hpp"
#include "ControllerUtils.hpp"

#include <sstream>
#include <iostream>
#include <cmath>
#include <iomanip>

#include <chrono>

using namespace std::chrono_literals;

SensorController::SensorController(const std::shared_ptr<oatpp::web::mime::ContentMappers>& apiContentMappers,
                                   std::shared_ptr<ISystemModule> systemModule)
    : SMBRControllerBase(apiContentMappers, systemModule)
    , activeCapturePromise(nullptr)
{
    captureWorker = std::thread([this]() {
        while (true) {
            std::function<void()> task;

            {
                std::unique_lock<std::mutex> lock(queueMutex);
                //TODO timeout
                captureQueueCondition.wait(lock, [this]() { return stopCaptureWorker || !captureQueue.empty(); });

                if (stopCaptureWorker && captureQueue.empty()) break;

                task = std::move(captureQueue.front());
                captureQueue.pop();
            }

            task();
        }
    });
}

SensorController::~SensorController() {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        stopCaptureWorker = true;
        if (activeCapturePromise) {
            try {
                activeCapturePromise->set_exception(std::make_exception_ptr(std::runtime_error("Controller is being destroyed")));
            } catch (...) {
                std::cerr << "Exception caught while setting exception on activeCapturePromise" << std::endl;
            }
            activeCapturePromise.reset();
        }
    }
    captureQueueCondition.notify_all();
    if (captureWorker.joinable())
        captureWorker.join();
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SensorController::getBottleTemperature() {

    return process(__FUNCTION__, [&](){
        auto temperature = waitFor(systemModule->sensorModule()->getBottleTemperature());
        auto tempResponseDto = TempDto::createShared();
        tempResponseDto->temperature = temperature;
        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SensorController::getTopMeasuredTemperature() {

    return process(__FUNCTION__, [&](){
        auto temperature = waitFor(systemModule->sensorModule()->getTopMeasuredTemperature());
        auto tempResponseDto = TempDto::createShared();
        tempResponseDto->temperature = temperature;
        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SensorController::getBottomMeasuredTemperature() {

    return process(__FUNCTION__, [&](){
        auto temperature = waitFor(systemModule->sensorModule()->getBottomMeasuredTemperature());
        auto tempResponseDto = TempDto::createShared();
        tempResponseDto->temperature = temperature;
        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SensorController::getTopSensorTemperature() {

    return process(__FUNCTION__, [&](){
        auto temperature = waitFor(systemModule->sensorModule()->getTopSensorTemperature());
        auto tempResponseDto = TempDto::createShared();
        tempResponseDto->temperature = temperature;
        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SensorController::getBottomSensorTemperature() {

    return process(__FUNCTION__, [&](){
        auto temperature = waitFor(systemModule->sensorModule()->getBottomSensorTemperature());
        auto tempResponseDto = TempDto::createShared();
        tempResponseDto->temperature = temperature;
        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SensorController::clearCustomText() {
    return processBool(__FUNCTION__, [&](){
        return waitFor(systemModule->sensorModule()->clearCustomText());
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response>
SensorController::printCustomText(const oatpp::Object<TextDto>& body) {
    return processBool(__FUNCTION__, [&](){
        if (!body->text) {
            throw ArgumentException("missing or invalid field");
        }
        
        size_t textLength = body->text->size();
        if (textLength < 1 || textLength > 127) {
            throw ArgumentException("Text length must be between 1 and 127 characters.");
        }
        
        return waitFor(systemModule->sensorModule()->printCustomText(body->text));
    });
}

Fluorometer_config::Gain SensorController::getGain(const std::string& gainStr) {
    if (gainStr == "x1") {
        return Fluorometer_config::Gain::x1;
    } else if (gainStr == "x10") {
        return Fluorometer_config::Gain::x10;
    } else if (gainStr == "x50") {
        return Fluorometer_config::Gain::x50;
    } else if (gainStr == "Auto") {
        return Fluorometer_config::Gain::Auto;
    } else {
        throw NotFoundException("Gain not found: " + gainStr);
    }
}

Fluorometer_config::Timing SensorController::getTimebase(const std::string& timebaseStr) {
  if (timebaseStr == "linear") {
    return Fluorometer_config::Timing::Linear;
  } else if (timebaseStr == "logarithmic") {
    return Fluorometer_config::Timing::Logarithmic;
  } else {
    throw NotFoundException("Timing not found: " + timebaseStr);
  }
}


std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SensorController::performFluorometerSingleSample(
    const oatpp::Object<FluorometerSingleSampleRequestDto>& body
) {
    return process(__FUNCTION__, [&]() {
        if (!body->detector_gain || !body->emitor_intensity) {
            throw ArgumentException("Missing required parameters.");
        }
        if (body->emitor_intensity < 0.2f || body->emitor_intensity > 1.0f) {
            throw ArgumentException("Invalid emitor intensity. Must be between 0.2 and 1.0");
        }
        const std::unordered_set<std::string> validGains = {"x1", "x10", "x50", "Auto"};
        if (validGains.find(body->detector_gain) == validGains.end()) {
            throw ArgumentException("Invalid detector_gain. Must be one of: x1, x10, x50, Auto.");
        }

        auto sample = waitFor(systemModule->sensorModule()->takeFluorometerSingleSample(getGain(body->detector_gain), body->emitor_intensity));

        auto responseDto = FluorometerSingleSampleResponseDto::createShared();
        responseDto->raw_value = sample.raw_value;
        responseDto->relative_value = sample.relative_value;
        responseDto->absolute_value = sample.absolute_value;

        return createDtoResponse(Status::CODE_200, responseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SensorController::captureFluorometerOjip(
    const oatpp::Object<FluorometerOjipCaptureRequestDto>& body
) {
    if (!body->detector_gain || !body->timebase || !body->emitor_intensity || !body->length_ms || !body->sample_count) {
        auto dto = MessageDto::createShared();
        dto->message = "Missing required parameters.";
        return createDtoResponse(Status::CODE_400, dto);
    }
    if (body->length_ms < 200 || body->length_ms > 4000) {
        auto dto = MessageDto::createShared();
        dto->message = "Invalid length. Must be between 200 and 4000.";
        return createDtoResponse(Status::CODE_400, dto);
    }
    if (body->sample_count  < 200 || body->sample_count  > 4000) {
        auto dto = MessageDto::createShared();
        dto->message = "Invalid sample count. Must be between 200 and 4000.";
        return createDtoResponse(Status::CODE_400, dto);
    }
    if (body->emitor_intensity < 0.2f || body->emitor_intensity > 1.0f) {
        auto dto = MessageDto::createShared();
        dto->message = "Invalid emitor intensity. Must be between 0.2 and 1.0.";
        return createDtoResponse(Status::CODE_400, dto);
    }

    const std::unordered_set<std::string> validGains = {"x1", "x10", "x50", "Auto"};
    const std::unordered_set<std::string> validTimebases = {"logarithmic", "linear"};
    if (validGains.find(body->detector_gain) == validGains.end()) {
        auto dto = MessageDto::createShared();
        dto->message = "Invalid detector_gain. Must be one of: x1, x10, x50, Auto.";
        return createDtoResponse(Status::CODE_400, dto);
    }
    if (validTimebases.find(body->timebase) == validTimebases.end()) {
        auto dto = MessageDto::createShared();
        dto->message = "Invalid timebase. Must be one of: logarithmic, linear.";
        return createDtoResponse(Status::CODE_400, dto);
    }

    auto promise = std::make_shared<std::promise<std::shared_ptr<oatpp::web::protocol::http::outgoing::Response>>>();
    auto future = std::make_shared<std::shared_future<std::shared_ptr<oatpp::web::protocol::http::outgoing::Response>>>(promise->get_future().share());

    {
        std::lock_guard<std::mutex> lock(activeCaptureMutex);
        activeCaptureFuture = future;
    }
    {
        std::lock_guard<std::mutex> lock(queueMutex);

        if (captureQueue.size() >= MAX_QUEUE_SIZE) {
            auto dto = MessageDto::createShared();
            dto->message = "Capture queue is full, try again later.";
            return createDtoResponse(Status::CODE_429, dto); 
        }
        captureQueue.push([this, promise, body]() {
            try {
                {
                    std::unique_lock<std::mutex> retrieveLock(retrieveMutex);
                    //TODO timeout
                    retrieveCondition.wait(retrieveLock, [this]() {
                        return !retrievingInProgress;
                    });
                }
                std::unique_lock<std::mutex> captureLock(captureMutex);

                ISensorModule::FluorometerInput input{
                    .detector_gain = getGain(body->detector_gain),
                    .sample_timebase = getTimebase(body->timebase),
                    .emitor_intensity = body->emitor_intensity,
                    .length_ms = body->length_ms,
                    .sample_count = body->sample_count
                };

                //TODO specify timeout
                auto fluorometerData = waitFor(systemModule->sensorModule()->captureFluorometerOjip(input));

                auto ojipDataDto = FluorometerMeasurementDto::createShared();
                ojipDataDto->measurement_id = fluorometerData.measurement_id;
                ojipDataDto->detector_gain = static_cast<dto::GainEnum>(fluorometerData.detector_gain);
                ojipDataDto->timebase = static_cast<dto::TimebaseEnum>(fluorometerData.timebase);
                ojipDataDto->emitor_intensity = fluorometerData.emitor_intensity;
                ojipDataDto->length_ms = fluorometerData.length_ms;
                ojipDataDto->required_samples = fluorometerData.required_samples;
                ojipDataDto->captured_samples = fluorometerData.captured_samples;
                ojipDataDto->missing_samples = fluorometerData.missing_samples;
                ojipDataDto->read = fluorometerData.read;
                ojipDataDto->saturated = fluorometerData.saturated;
                ojipDataDto->timestamp = fluorometerData.iso_start_time;

                oatpp::Vector<oatpp::Object<FluorometerSampleDto>> samples = oatpp::Vector<oatpp::Object<FluorometerSampleDto>>::createShared();
                for (const auto& sample : fluorometerData.samples) {
                    auto sampleDto = FluorometerSampleDto::createShared();
                    sampleDto->time_ms = sample.time_ms;
                    sampleDto->raw_value = sample.raw_value;
                    sampleDto->relative_value = sample.relative_value;
                    sampleDto->absolute_value = sample.absolute_value;
                    samples->push_back(sampleDto);
                }
                ojipDataDto->samples = samples;

                promise->set_value(createDtoResponse(Status::CODE_200, ojipDataDto));
            } catch (const std::exception& e) {
                auto dto = MessageDto::createShared();
                dto->message = "Error during fluorometer capture: " + std::string(e.what());
                promise->set_value(createDtoResponse(Status::CODE_504, dto));
            }

            {
                std::lock_guard<std::mutex> lock(activeCaptureMutex);
                activeCaptureFuture.reset();
            }
        });
    }

    captureQueueCondition.notify_all();
    return future->get();
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SensorController::checkFluorometerOjipCaptureComplete() {
    return process(__FUNCTION__, [&](){
        //TODO specify timeout
        auto captureComplete = waitFor(systemModule->sensorModule()->isFluorometerOjipCaptureComplete());
        auto captureStatusDto = FluorometerCaptureStatusDto::createShared();
        captureStatusDto->capture_complete = captureComplete;
        return createDtoResponse(Status::CODE_200, captureStatusDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SensorController::retrieveLastFluorometerOjipData() {

    {
        std::lock_guard<std::mutex> lock(activeCaptureMutex);
        if (activeCaptureFuture) {
            try {
                return activeCaptureFuture->get();
            } catch (const std::exception& e) {
                return createResponse(Status::CODE_500, e.what());
            }
        }
    }

    std::unique_lock<std::mutex> lock(retrieveMutex);

    if (retrievingInProgress) {
        //TODO timeout
        retrieveCondition.wait(lock, [this]() {
            return !retrievingInProgress;
        });

        return cachedResponse;
    }

    retrievingInProgress = true;
    cachedResponse = nullptr;
    lock.unlock();

    auto response = process(__FUNCTION__, [&]() {
        //TODO timeout
        auto fluorometerData = waitFor(systemModule->sensorModule()->retrieveLastFluorometerOjipData());

        auto dto = FluorometerMeasurementDto::createShared();
        dto->measurement_id = fluorometerData.measurement_id;
        dto->detector_gain = static_cast<dto::GainEnum>(fluorometerData.detector_gain);
        dto->timebase = static_cast<dto::TimebaseEnum>(fluorometerData.timebase);
        dto->emitor_intensity = fluorometerData.emitor_intensity;
        dto->length_ms = fluorometerData.length_ms;
        dto->required_samples = fluorometerData.required_samples;
        dto->captured_samples = fluorometerData.captured_samples;
        dto->missing_samples = fluorometerData.missing_samples;
        dto->read = fluorometerData.read;
        dto->saturated = fluorometerData.saturated;
        dto->timestamp = fluorometerData.iso_start_time;

        oatpp::Vector<oatpp::Object<FluorometerSampleDto>> samples = oatpp::Vector<oatpp::Object<FluorometerSampleDto>>::createShared();
        for (const auto& sample : fluorometerData.samples) {
            auto sampleDto = FluorometerSampleDto::createShared();
            sampleDto->time_ms = sample.time_ms;
            sampleDto->raw_value = sample.raw_value;
            sampleDto->relative_value = sample.relative_value;
            sampleDto->absolute_value = sample.absolute_value;
            samples->push_back(sampleDto);
        }
        dto->samples = samples;

        return createDtoResponse(Status::CODE_200, dto);
    });

    lock.lock();
    cachedResponse = response;
    retrievingInProgress = false;
    retrieveCondition.notify_all();
    return response;
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SensorController::getFluorometerDetectorInfo() {
    return process(__FUNCTION__, [&](){
        auto info = waitFor(systemModule->sensorModule()->getFluorometerDetectorInfo());
        auto infoDto = FluorometerDetectorInfoDto::createShared();
        infoDto->peak_wavelength = info.peak_wavelength;
        infoDto->sensitivity = info.sensitivity;
        infoDto->sampling_rate = info.sampling_rate;
        return createDtoResponse(Status::CODE_200, infoDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SensorController::getFluorometerDetectorTemperature() {
    return process(__FUNCTION__, [&](){
        auto temperature = waitFor(systemModule->sensorModule()->getFluorometerDetectorTemperature());
        auto tempResponseDto = TempDto::createShared();
        tempResponseDto->temperature = temperature;
        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SensorController::getFluorometerEmitorInfo() {
    return process(__FUNCTION__, [&](){
        auto info = waitFor(systemModule->sensorModule()->getFluorometerEmitorInfo());
        auto infoDto = FluorometerEmitorInfoDto::createShared();
        infoDto->peak_wavelength = info.peak_wavelength;
        infoDto->power_output = info.power_output;
        return createDtoResponse(Status::CODE_200, infoDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SensorController::getFluorometerEmitorTemperature() {
    return process(__FUNCTION__, [&](){
        auto temperature = waitFor(systemModule->sensorModule()->getFluorometerEmitorTemperature());
        auto tempResponseDto = TempDto::createShared();
        tempResponseDto->temperature = temperature;
        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SensorController::calibrateFluorometer(const oatpp::String& body)  {
    return processBool(__FUNCTION__, [&](){
        if (body != "{}") {
            throw ArgumentException("Invalid input. Expected empty JSON object {}");
        }
        return waitFor(systemModule->sensorModule()->calibrateFluorometer());
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SensorController::getSpectrophotometerChannels() {
    return process(__FUNCTION__, [&](){
        auto channels = waitFor(systemModule->sensorModule()->getSpectrophotometerChannels());
        auto responseDto = SpectroChannelsDto::createShared();
        responseDto->channels = channels;
        return createDtoResponse(Status::CODE_200, responseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SensorController::getSpectrophotometerChannelInfo(const oatpp::UInt8& channel) {
    return process(__FUNCTION__, [&](){
        auto channelInfo = waitFor(systemModule->sensorModule()->getSpectrophotometerChannelInfo(channel));
        auto responseDto = SpectroChannelInfoDto::createShared();
        responseDto->channel = channelInfo.channel;
        responseDto->peak_wavelength = channelInfo.peak_wavelength;
        responseDto->half_intensity_peak_width = channelInfo.half_intensity_peak_width;
        return createDtoResponse(Status::CODE_200, responseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SensorController::measureAllSpectrophotometerChannels() {
    return process(__FUNCTION__, [&](){
        auto channelCount = waitFor(systemModule->sensorModule()->getSpectrophotometerChannels());

        auto measurementsDto = SpectroMeasurementsDto::createShared();
        measurementsDto->samples = oatpp::Vector<oatpp::Object<SingleChannelMeasurementDto>>::createShared();

        for (int8_t channel = 0; channel < channelCount; channel++) {
            auto measurement = waitFor(systemModule->sensorModule()->measureSpectrophotometerChannel(channel));
            auto channelMeasurement = SingleChannelMeasurementDto::createShared();
            channelMeasurement->channel = measurement.channel;
            channelMeasurement->relative_value = measurement.relative_value;
            channelMeasurement->absolute_value = measurement.absolute_value;
            measurementsDto->samples->push_back(channelMeasurement);
        }

        return createDtoResponse(Status::CODE_200, measurementsDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SensorController::measureSingleSpectrophotometerChannel(const Int8& channel) {
    return process(__FUNCTION__, [&](){
        auto measurement = waitFor(systemModule->sensorModule()->measureSpectrophotometerChannel(channel));
        auto responseDto = SingleChannelMeasurementDto::createShared();
        responseDto->channel = measurement.channel;
        responseDto->relative_value = measurement.relative_value;
        responseDto->absolute_value = measurement.absolute_value;
        return createDtoResponse(Status::CODE_200, responseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SensorController::getSpectrophotometerEmitorTemperature() {
    return process(__FUNCTION__, [&](){
        auto temperature = waitFor(systemModule->sensorModule()->getSpectrophotometerEmitorTemperature());
        auto tempResponseDto = TempDto::createShared();
        tempResponseDto->temperature = temperature;
        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SensorController::calibrateSpectrophotometer(const oatpp::String& body)  {
    return processBool(__FUNCTION__, [&](){
        if (body != "{}") {
            throw ArgumentException("Invalid input. Expected empty JSON object {}");
        }
        return waitFor(systemModule->sensorModule()->calibrateSpectrophotometer());
    });
}
