#pragma once

#include "BaseController.hpp"

#include "dto/CaptureEnumDto.hpp"
#include "dto/FluorometerCaptureStatusDto.hpp"
#include "dto/FluorometerDetectorInfoDto.hpp"
#include "dto/FluorometerEmitorInfoDto.hpp"
#include "dto/FluorometerMeasurementDto.hpp"
#include "dto/FluorometerOjipCaptureRequestDto.hpp"
#include "dto/FluorometerSampleDto.hpp"
#include "dto/FluorometerSingleSampleRequestDto.hpp"
#include "dto/FluorometerSingleSampleResponseDto.hpp"
#include "dto/MessageDto.hpp"
#include "dto/SingleChannelMeasurementDto.hpp"
#include "dto/SpectroCalibrateDto.hpp"
#include "dto/SpectroChannelInfoDto.hpp"
#include "dto/SpectroChannelsDto.hpp"
#include "dto/SpectroMeasurementsDto.hpp"
#include "dto/TempDto.hpp"
#include "dto/TextDto.hpp"

#include <atomic>
#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>
#include <thread>

#include OATPP_CODEGEN_BEGIN(ApiController)

#undef OATPP_MACRO_API_CONTROLLER_ADD_CORS_BODY_DEFAULT_METHODS
#define OATPP_MACRO_API_CONTROLLER_ADD_CORS_BODY_DEFAULT_METHODS "GET, POST, OPTIONS, PUT, PATCH, DELETE"

/**
 * @class SensorController
 * @brief Endpoints of the sensor module.
 */
class SensorController : public SMBRControllerBase {
public:
    SensorController(const std::shared_ptr<oatpp::web::mime::ContentMappers>& apiContentMappers,
                     std::shared_ptr<ISystemModule> systemModule);
    ~SensorController();

    /**
     * @brief Retrieves the temperature of the bottle.
     */
    ENDPOINT_INFO(getBottleTemperature) {
        info->summary = "Retrieves temperature of the bottle";
        info->description = 
            "Retrieves temperature of the bottle in °C."
            "This temperature could be fusion of several sensors measuring bottle."
            "At default it is fusion of top and bottom thermopile but more sensors can be added."
            "This temperature is generally more filtered in comparison to top and bottom thermopile temperatures.";
        info->addTag("Sensor module");
        auto example = TempDto::createShared();
        example->temperature = 30.2f; 
        info->addResponse<Object<TempDto>>(Status::CODE_200, "application/json", "Successfully retrieved temperature of the bottle")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve temperature")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve temperature"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getBottleTemperature)
    ENDPOINT("GET", "/sensor/bottle/temperature", getBottleTemperature);

    /**
     * @brief Retrieves the measured temperature from the top sensor of the bottle.
     */
    ENDPOINT_INFO(getTopMeasuredTemperature) {
        info->summary = "Retrieves measured temperature from top sensor";
        info->description = 
            "Retrieves measured temperature of the top of the bottle in °C."
            "This temperature is measured by thermopile sensor placed on top of right side of the bottle."
            "This temperature has low filtration of it's value and can be used for fast temperature changes.";
        info->addTag("Sensor module");
        auto example = TempDto::createShared();
        example->temperature = 30.2f; 
        info->addResponse<Object<TempDto>>(Status::CODE_200, "application/json", "Successfully retrieved measured temperature from top thermopile")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve temperature")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve temperature"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getTopMeasuredTemperature)
    ENDPOINT("GET", "/sensor/bottle/top/measured_temperature", getTopMeasuredTemperature);

    /**
     * @brief Retrieves the temperature of the top sensor case of the bottle.
     */
    ENDPOINT_INFO(getTopSensorTemperature) {
        info->summary = "Retrieves temperature of the top sensor case";
        info->description = 
            "Retrieves temperature of the sensor on top of the bottle in °C."
            "This temperature is measured internally inside case of the sensor."
            "This value is already used as cold side compensation in measured temperature of the sensor."
            "Could be sued to detect external effects on bottle or distortions due to high sensor temperature."
            "This sensor could be heated by other components on sensor board, so it is not suitable for ambient temperature measurement.";
        info->addTag("Sensor module");
        auto example = TempDto::createShared();
        example->temperature = 30.2f; 
        info->addResponse<Object<TempDto>>(Status::CODE_200, "application/json", "Successfully retrieved temperature of the top sensor case")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve temperature")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve temperature"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getTopSensorTemperature)
    ENDPOINT("GET", "/sensor/bottle/top/sensor_temperature", getTopSensorTemperature);

    /**
     * @brief Retrieves the measured temperature from the bottom sensor of the bottle.
     */
    ENDPOINT_INFO(getBottomMeasuredTemperature) {
        info->summary = "Retrieves measured temperature from bottom sensor";
        info->description = 
            "Retrieves measured temperature of the bottom of the bottle in °C."
            "This temperature is measured by thermopile sensor placed on bottom of right side of the bottle."
            "This temperature has low filtration of it's value and can be used for fast temperature changes.";
        info->addTag("Sensor module"); 
        auto example = TempDto::createShared();
        example->temperature = 30.2f; 
        info->addResponse<Object<TempDto>>(Status::CODE_200, "application/json", "Successfully retrieved measured temperature from bottom thermopile")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve temperature")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve temperature"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getBottomMeasuredTemperature)
    ENDPOINT("GET", "/sensor/bottle/bottom/measured_temperature", getBottomMeasuredTemperature);

    /**
     * @brief Retrieves the temperature of the bottom sensor case of the bottle.
     */
    ENDPOINT_INFO(getBottomSensorTemperature) {
        info->summary = "Retrieves temperature of the bottom sensor case";
        info->description = 
            "Retrieves temperature of the sensor on bottom of the bottle in °C."
            "This temperature is measured internally inside case of the sensor."
            "This value is already used as cold side compensation in measured temperature of the sensor."
            "Could be sued to detect external effects on bottle or distortions due to high sensor temperature."
            "This sensor could be heated by other components on sensor board, so it is not suitable for ambient temperature measurement.";
        info->addTag("Sensor module");
        auto example = TempDto::createShared();
        example->temperature = 30.2f; 
        info->addResponse<Object<TempDto>>(Status::CODE_200, "application/json", "Successfully retrieved temperature of the bottom sensor case")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve temperature")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve temperature"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getBottomSensorTemperature)
    ENDPOINT("GET", "/sensor/bottle/bottom/sensor_temperature", getBottomSensorTemperature);

    /**
     * @brief Clears custom text on Mini OLED display and displays the serial number.
     */
    ENDPOINT_INFO(clearCustomText) {
        info->summary = "Clear custom text on Mini OLED display";
        info->description = 
            "Clear custom text on Mini OLED display and display serial number of the device."
            "If no text is set to display, serial number of device will be displayed instead.";
        info->addTag("Sensor module");
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Successfully cleared custom text on Mini OLED display")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Successfully cleared custom text on Mini OLED display"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to clear custom text")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to clear custom text"}}));
    }
    ADD_CORS(clearCustomText)
    ENDPOINT("GET", "/sensor/oled/clear_custom_text", clearCustomText);

    /**
     * @brief Prints custom text on Mini OLED display.
     */
    ENDPOINT_INFO(printCustomText) {
        info->summary = "Print custom text on Mini OLED display";
        info->description = 
            "Use last line of Mini OLED display to print custom text. Text posted to this endpoint will be appended to existing text."
            "To clear displayed text use clear_custom_text endpoint. If no text is set to display, serial number if device will be displayed instead."
            "If text is longer then width of display then it will be scrolled.";
        info->addTag("Sensor module");
        auto example = TextDto::createShared();
        example->text = "Hello"; 
        info->addConsumes<Object<TextDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Successfully set text to OLED display")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Successfully set text to OLED display"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Invalid request body")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Invalid request body"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to set text on OLED display")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to set text on OLED display"}}));
    }
    ADD_CORS(printCustomText)
    ENDPOINT("POST", "/sensor/oled/print_custom_text", printCustomText, BODY_DTO(Object<TextDto>, dto));

    /**
     * @brief Perform single sample measurement on fluorometer.
     */
    ENDPOINT_INFO(performFluorometerSingleSample) {
        info->summary = "Perform single sample measurement on fluorometer";
        info->description = 
            "Perform single sample of data measurement on fluorometer."
            "This will start capturing of single sample and immediately retrieve it."
            "Detector gain and emitor intensity can be used to change relative range of measured values."
            "Emitor intensity is not strictly linear, so it is better to use gain of detector to change range."
            "Absolute value is calculated based on gain of detector and emitor intensity. And relates to detector max range.\n"
            "\n"
            "Allowed values:\n"
            "- detector_gain: \"x1\", \"x10\", \"x50\", \"Auto\"\n"
            "- emitor_intensity: 0.2 to 1.0";
        info->addTag("Sensor module");

        auto exampleRequest = FluorometerSingleSampleRequestDto::createShared();
        exampleRequest->emitor_intensity = 0.5f;
        exampleRequest->detector_gain = "x1";
        
        auto exampleResponse = FluorometerSingleSampleResponseDto::createShared();
        exampleResponse->raw_value = 1023;
        exampleResponse->relative_value = 0.25f;
        exampleResponse->absolute_value = 0.125f;

        info->addConsumes<Object<FluorometerSingleSampleRequestDto>>("application/json")
            .addExample("application/json", exampleRequest);
        info->addResponse<Object<FluorometerSingleSampleResponseDto>>(Status::CODE_200, "application/json", "Successfully retrieved single sample values")
            .addExample("application/json", exampleResponse);
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Invalid request body")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Invalid request body"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to perform measurement")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to perform measurement"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(performFluorometerSingleSample)
    ENDPOINT("POST", "/sensor/fluorometer/single_sample", performFluorometerSingleSample, 
        BODY_DTO(Object<FluorometerSingleSampleRequestDto>, body));

    /**
     * @brief Perform OJIP capture on fluorometer.
     */
    ENDPOINT_INFO(captureFluorometerOjip) {
        info->summary = "Perform OJIP capture on fluorometer";
        info->description = 
            "Start OJIP capture on fluorometer and return captured data."
            "This will start capturing the OJIP curve and store it in the memory of the fluorometer."
            "After the capture is completed, it will read the data and return it."
            "During measurement, some components are unable to be used, like density measurements, and their processing can be postponed."
            "Detector gain and emitter intensity can be used to change the relative range of measured values."
            "This can lead to better relative resolution, with regard to the max value of the OJIP curve."
            "But this can also lead to saturation of the detector and loss of data."
            "Emitter intensity is not strictly linear, so it is better to use the gain of the detector to change the range."
            "Absolute value is calculated based on the gain of the detector and emitter intensity. And relates to the detector's max range."
            "More info about the returned data can be found in the read_last endpoint description.\n\n"
            "Expected timeout can be estimated as:\n"
            "timeout_ms ≈ length_ms + 2 × required_samples (in milliseconds).\n"
            "\n"
            "Allowed values:\n"
            "- detector_gain: \"x1\", \"x10\", \"x50\", \"Auto\"\n"
            "- emitor_intensity: 0.2 to 1.0\n"
            "- timebase: \"linear\", \"logarithmic\"\n"
            "- length_ms: 200 to 4000 ms\n"
            "- sample_count: 200 to 4000 samples";
        info->addTag("Sensor module");

        auto example = FluorometerMeasurementDto::createShared();
        example->measurement_id = 1235;
        example->read = false;
        example->saturated = false;
        example->detector_gain = dto::GainEnum::x1;
        example->emitor_intensity = 0.5;
        example->timebase = dto::TimebaseEnum::logarithmic;
        example->timestamp = "2025-05-30T12:34:56";
        example->length_ms = 1000;
        example->required_samples = 1000;
        example->captured_samples = 998;
        example->missing_samples = 2;

        
        auto sample1 = FluorometerSampleDto::createShared();
        sample1->time_ms = 1.2f; 
        sample1->raw_value = 1023; 
        sample1->relative_value = 0.5f; 
        sample1->absolute_value = 0.125f;

        auto sample2 = FluorometerSampleDto::createShared();
        sample2->time_ms = 10.0f;
        sample2->raw_value = 4095;
        sample2->relative_value = 1.0f;
        sample2->absolute_value = 0.95f;

        auto example2 = FluorometerOjipCaptureRequestDto::createShared();
        example2->detector_gain = "x1";
        example2->emitor_intensity = 1.0;
        example2->timebase = "logarithmic";
        example2->length_ms = 1000;
        example2->sample_count = 1000;

        example->samples = {sample1, sample2};

        info->addConsumes<Object<FluorometerOjipCaptureRequestDto>>("application/json")
            .addExample("application/json", example2);
        info->addResponse<Object<FluorometerMeasurementDto>>(Status::CODE_200, "application/json", "Successfully retrieved OJIP data from fluorometer")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Invalid request body")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Invalid request body"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "OJIP data not available")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "OJIP data not available"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to perform OJIP capture")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to perform OJIP capture"}}));
    }

    ADD_CORS(captureFluorometerOjip)
    ENDPOINT("POST", "/sensor/fluorometer/ojip/capture", captureFluorometerOjip,
        BODY_DTO(Object<FluorometerOjipCaptureRequestDto>, body));

    /**
     * @brief Checks if fluorometer OJIP capture is complete.
     */
    ENDPOINT_INFO(checkFluorometerOjipCaptureComplete) {
        info->summary = "Check if fluorometer OJIP capture is complete";
        info->description = "Check if fluorometer capture is complete and data is ready to be retrieved.";
        info->addTag("Sensor module");
        auto example = FluorometerCaptureStatusDto::createShared();
        example->capture_complete = false; 
        info->addResponse<Object<FluorometerCaptureStatusDto>>(Status::CODE_200, "application/json", "Successfully checked fluorometer capture status")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to check capture status")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to check capture status"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }

    ADD_CORS(checkFluorometerOjipCaptureComplete)
    ENDPOINT("GET", "/sensor/fluorometer/ojip/completed", checkFluorometerOjipCaptureComplete);

/**
     * @brief Retrieve OJIP data from fluorometer.
     */
    ENDPOINT_INFO(retrieveLastFluorometerOjipData) {
        info->summary = "Retrieve OJIP data from last fluorometer capture";
        info->description = 
        "Retrieve OJIP data from last fluorometer capture"
        "Measurement ID is generally increasing in value and different for every new measurement started."
        "If measurement is retrieved twice ID will be same."
        "Measurement ID is used to determine if this measurement is already exported to database."
        "Boolean Read contains information if data was already read from sensor."
        "Starts as true and resets with every new measurement started."
        "Serves as easier way to determine if data was already read instead of Measurement ID.";
        "Second retrieve without starting new measurement will return false."
        "Samples are always ordered by time of capture."
        "Time between capture_start and retrieve should be at least double of length of measurement."
        "Based on selected parameters when measurement was started, data can be saturated."
        "Based on this gain of detector or intensity of emitor can be changed for next measurement."
        "Intensity of emitor is not strictly linear, so it is better to use gain of detector to change absolute values."
        "Samples has 3 types of values."
          "- raw_value: raw adc value from detector (generaly 12-bit, see detector info)"
          "- relative_value: value normalized to 0-1 range"
          "- absolute_value: value normalized to 0-1 range and corrected for gain of detector and emitor intensity."
        "\n"
        "Expected timeout can be estimated as:\n"
        "timeout_ms ≈ 2 × required_samples (in milliseconds).\n";
        info->addTag("Sensor module");
        auto example = FluorometerMeasurementDto::createShared();
        example->measurement_id = 1235;
        example->read = false;
        example->saturated = false;
        example->detector_gain = dto::GainEnum::x1;
        example->emitor_intensity = 0.5;
        example->timebase = dto::TimebaseEnum::logarithmic;
        example->timestamp = "2025-05-30T12:34:56";
        example->length_ms = 1000;
        example->required_samples = 1000;
        example->captured_samples = 998;
        example->missing_samples = 2;

        
        auto sample1 = FluorometerSampleDto::createShared();
        sample1->time_ms = 1.2f; 
        sample1->raw_value = 1023; 
        sample1->relative_value = 0.5f; 
        sample1->absolute_value = 0.125f; 

        auto sample2 = FluorometerSampleDto::createShared();
        sample2->time_ms = 10.0f;
        sample2->raw_value = 4095;
        sample2->relative_value = 1.0f;
        sample2->absolute_value = 0.95f;

        example->samples = {sample1, sample2};
        info->addResponse<Object<FluorometerMeasurementDto>>(Status::CODE_200, "application/json", "Successfully retrieved OJIP data from fluorometer")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "OJIP data not available")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "OJIP data not available"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve OJIP data")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve OJIP data"}}));
    }

    ADD_CORS(retrieveLastFluorometerOjipData)
    ENDPOINT("GET", "/sensor/fluorometer/ojip/read_last", retrieveLastFluorometerOjipData);

    /**
     * @brief Retrieves information about the fluorometer detector.
     */
    ENDPOINT_INFO(getFluorometerDetectorInfo) {
        info->summary = "Retrieves information about the fluorometer detector";
        info->description = "Contains:\n"
                            "- detector peak wavelength in nm\n"
                            "- detector sensitivity\n"
                            "- sampling rate in kHz";
        info->addTag("Sensor module");
        auto example = FluorometerDetectorInfoDto::createShared();
        example->peak_wavelength = 750;
        example->sensitivity = 100;
        example->sampling_rate = 500;
        info->addResponse<Object<FluorometerDetectorInfoDto>>(Status::CODE_200, "application/json", "Successfully retrieved information about the fluorometer")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve detector info")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve detector info"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getFluorometerDetectorInfo)
    ENDPOINT("GET", "/sensor/fluorometer/detector/info", getFluorometerDetectorInfo);

    /**
     * @brief Retrieves the temperature of the fluorometer detector.
     */
    ENDPOINT_INFO(getFluorometerDetectorTemperature) {
        info->summary = "Retrieves temperature of the fluorometer detector";
        info->description = 
            "Retrieves temperature of the fluorometer detector in °C."
            "High temperature (>40°C) can have negative effect on detector sensitivity.";
        info->addTag("Sensor module");
        auto example = TempDto::createShared();
        example->temperature = 25.5f; 
        info->addResponse<Object<TempDto>>(Status::CODE_200, "application/json", "Successfully retrieved temperature of the fluorometer detector")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve temperature")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve temperature"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getFluorometerDetectorTemperature)
    ENDPOINT("GET", "/sensor/fluorometer/detector/temperature", getFluorometerDetectorTemperature);

    /**
     * @brief Retrieves information about the fluorometer emitor.
     */
    ENDPOINT_INFO(getFluorometerEmitorInfo) {
        info->summary = "Retrieves information about the fluorometer emitor";
        info->description = 
            "Contains:\n"
            "  - peak wavelength in nm\n"
            "  - max power output in mW";
        info->addTag("Sensor module");
        auto example = FluorometerEmitorInfoDto::createShared();
        example->peak_wavelength = 525;
        example->power_output = 10000;
        info->addResponse<Object<FluorometerEmitorInfoDto>>(Status::CODE_200, "application/json", "Successfully retrieved information about the fluorometer emitor")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve emitor info")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve emitor info"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getFluorometerEmitorInfo)
    ENDPOINT("GET", "/sensor/fluorometer/emitor/info", getFluorometerEmitorInfo);

    /**
     * @brief Retrieves the temperature of the fluorometer emitor.
     */
    ENDPOINT_INFO(getFluorometerEmitorTemperature) {
        info->summary = "Retrieves temperature of the fluorometer emitor";
        info->description = 
            "Retrieves temperature of the fluorometer emitor in °C."
            "High temperature (>60°C) can have negative effect on emitor stability."
            "Measurement performed in quick succession can lead to higher temperature.";
        info->addTag("Sensor module");
        auto example = TempDto::createShared();
        example->temperature = 35.0f; 
        info->addResponse<Object<TempDto>>(Status::CODE_200, "application/json", "Successfully retrieved temperature of the fluorometer emitor")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve temperature")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve temperature"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getFluorometerEmitorTemperature)
    ENDPOINT("GET", "/sensor/fluorometer/emitor/temperature", getFluorometerEmitorTemperature);

    /**
     * @brief Request self-calibration of fluorometer.
     */
    ENDPOINT_INFO(calibrateFluorometer) {
        info->summary = "Calibrate fluorometer";
        info->description = 
            "Request self-calibration of fluorometer. For calibration cuvette should be empty or filled with clean medium."
            "No algae or other particles should be present in cuvette. Calibration is preserved in persistent memory of module."
            "And will be loaded during next power up. Body of request must be empty, but it is required to be present for future calibration extensions.";
        info->addTag("Sensor module");
        
        auto exampleRequest = SpectroCalibrateDto::createShared();
        exampleRequest->calibrationMode=nullptr;
        
        info->addConsumes<Object<SpectroCalibrateDto>>("application/json")
            .addExample("application/json", exampleRequest);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Calibration started")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Calibration started"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Invalid request body")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Invalid request body"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to start calibration")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to start calibration"}}));

    }
    ADD_CORS(calibrateFluorometer)
    ENDPOINT("POST", "/sensor/fluorometer/calibrate", calibrateFluorometer, BODY_STRING(String, requestBody));

    /**
     * @brief Reads number of channels available on spectrophotometer.
     */
    ENDPOINT_INFO(getSpectrophotometerChannels) {
        info->summary = "Reads number of channels available on spectrophotometer";
        info->description = 
            "Reads number of channels available on spectrophotometer."
            "Each channel can be used to measure absorbance of different wavelength."
            "Endpoints return N channels which are numbered from 0 to (N-1).";
        info->addTag("Sensor module");
        auto example = SpectroChannelsDto::createShared();
        example->channels = 6; 
        info->addResponse<Object<SpectroChannelsDto>>(Status::CODE_200, "application/json", "Successfully retrieved number of channels")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve channels count")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve channels count"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getSpectrophotometerChannels)
    ENDPOINT("GET", "/sensor/spectrophotometer/channels", getSpectrophotometerChannels);

    /**
     * @brief Read information about spectrophotometer channel.
     */
    ENDPOINT_INFO(getSpectrophotometerChannelInfo) {
        info->summary = "Read information about spectrophotometer channel";
        info->description = 
            "Read information about spectrophotometer channel.\n"
            "Contains:\n"
            "  - Peak wavelength in nm\n"
            "  - Half intensity peak width in nm";
        info->addTag("Sensor module");
        
        auto example = SpectroChannelInfoDto::createShared();
        example->channel = 1;
        example->peak_wavelength = 480;
        example->half_intensity_peak_width = 10;
        
        info->addResponse<Object<SpectroChannelInfoDto>>(Status::CODE_200, "application/json", "Successfully retrieved information about selected channel")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve channel info")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve channel info"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getSpectrophotometerChannelInfo)
    ENDPOINT("GET", "/sensor/spectrophotometer/channel_info/{channel}", getSpectrophotometerChannelInfo, PATH(oatpp::UInt8, channel));

    /**
     * @brief Measure all channels at once and return responses.
     */
    ENDPOINT_INFO(measureAllSpectrophotometerChannels) {
        info->summary = "Measure all channels at once and return responses";
        info->description = 
            "Measure all channels at once and return responses. Each channel can be used to measure absorbance of different wavelength."
            "Endpoints return N channel which are numbered from 0 to (N-1).";
        info->addTag("Sensor module");
        
        auto example = SpectroMeasurementsDto::createShared();
        example->samples = oatpp::Vector<oatpp::Object<SingleChannelMeasurementDto>>::createShared();

        auto addExampleMeasurement = [&](int8_t channel, float relative, uint16_t absolute) {
            auto measurement = SingleChannelMeasurementDto::createShared();
            measurement->channel = channel;
            measurement->relative_value = relative;
            measurement->absolute_value = absolute;
            example->samples->push_back(measurement);
        };

        addExampleMeasurement(0, 0.125f, 28);
        addExampleMeasurement(1, 0.236f, 2015);
        addExampleMeasurement(2, 0.180f, 38);
        addExampleMeasurement(3, 0.452f, 1115);
        addExampleMeasurement(4, 0.561f, 3845);
        addExampleMeasurement(5, 0.473f, 5724);
        
        info->addResponse<Object<SpectroMeasurementsDto>>(Status::CODE_200, "application/json", "Successfully performed measurement of all channels")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to perform measurements")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to perform measurements"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(measureAllSpectrophotometerChannels)
    ENDPOINT("POST", "/sensor/spectrophotometer/measure_all", measureAllSpectrophotometerChannels);

    /**
     * @brief Measure selected single channel and return response.
     */
    ENDPOINT_INFO(measureSingleSpectrophotometerChannel) {
        info->summary = "Measure selected single channel and return response";
        info->description = 
            "Measure selected single channel and return response. Each channel can be used to measure absorbance of different wavelength."
            "To get information about wavelength channel_info/{channel} could be used.";
        info->addTag("Sensor module");
        
        auto example = SingleChannelMeasurementDto::createShared();
        example->channel = 1;
        example->relative_value = 0.236f;
        example->absolute_value = 5012;
        
        info->addResponse<Object<SingleChannelMeasurementDto>>(Status::CODE_200, "application/json", "Successfully performed measurement of selected channel")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to perform measurement")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to perform measurement"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(measureSingleSpectrophotometerChannel)
    ENDPOINT("POST", "/sensor/spectrophotometer/measure/{channel}", measureSingleSpectrophotometerChannel, PATH(Int8, channel));

    /**
     * @brief Retrieves the temperature of the spectrophotometer emitor.
     */
    ENDPOINT_INFO(getSpectrophotometerEmitorTemperature) {
        info->summary = "Read temperature of the spectrophotometer emitor";
        info->description = "Retrieves the temperature of the spectrophotometer emitor in °C.";
        info->addTag("Sensor module");
        auto example = TempDto::createShared();
        example->temperature = 30.2f; 
        info->addResponse<Object<TempDto>>(Status::CODE_200, "application/json", "Successfully retrieved temperature of the spectrophotometer emitor")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve temperature")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve temperature"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getSpectrophotometerEmitorTemperature)
    ENDPOINT("GET", "/sensor/spectrophotometer/emitor/temperature", getSpectrophotometerEmitorTemperature);


    /**
     * @brief Request self-calibration of spectrophotometer.
     */
    ENDPOINT_INFO(calibrateSpectrophotometer) {
        info->summary = "Calibrate spectrophotometer";
        info->description = 
            "Request self-calibration of spectrophotometer. For calibration cuvette should be empty or filled with clean medium."
            "No algae or other particles should be present in cuvette. Calibration is preserved in persistent memory of module."
            "And will be loaded during next power up. Body of request must be empty, but it is required to be present for future calibration extensions.";
        info->addTag("Sensor module");
        
        auto exampleRequest = SpectroCalibrateDto::createShared();
        exampleRequest->calibrationMode=nullptr;
        
        info->addConsumes<Object<SpectroCalibrateDto>>("application/json")
            .addExample("application/json", exampleRequest);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Calibration started")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Calibration started"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Invalid request body")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Invalid request body"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to start calibration")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to start calibration"}}));

    }
    ADD_CORS(calibrateSpectrophotometer)
    ENDPOINT("POST", "/sensor/spectrophotometer/calibrate", calibrateSpectrophotometer, BODY_STRING(String, requestBody));

private:
    Fluorometer_config::Gain getGain(const std::string& gainStr);
    Fluorometer_config::Timing getTimebase(const std::string& timebaseStr);

    std::shared_ptr<std::promise<std::shared_ptr<oatpp::web::protocol::http::outgoing::Response>>> activeCapturePromise;
    std::thread captureWorker;
    std::condition_variable captureQueueCondition;
    std::atomic<bool> stopCaptureWorker = false;
    std::queue<std::function<void()>> captureQueue;
    static constexpr size_t MAX_QUEUE_SIZE = 4;
    std::mutex queueMutex;

    std::shared_ptr<std::shared_future<std::shared_ptr<oatpp::web::protocol::http::outgoing::Response>>> activeCaptureFuture;
    std::mutex activeCaptureMutex;
    std::mutex captureMutex;

    std::mutex retrieveMutex;
    std::condition_variable retrieveCondition;
    bool retrievingInProgress = false;
    std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> cachedResponse = nullptr;

};

#include OATPP_CODEGEN_END(ApiController)
