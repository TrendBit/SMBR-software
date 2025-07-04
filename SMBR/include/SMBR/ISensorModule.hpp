#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <future>
#include "SMBR/SMBR.hpp"
#include "../../SMBR.Can/Codes/include/codes/messages/fluorometer/fluorometer_config.hpp"

class ISensorModule {
public:
    typedef std::shared_ptr <ISensorModule> Ptr;

    virtual ~ISensorModule() = default;

    /**
     * @brief Structure representing input data of fluorometer capture.
     */
    struct FluorometerInput {
        Fluorometer_config::Gain detector_gain;
        Fluorometer_config::Timing sample_timing;
        float emitor_intensity = 1.0f;
        uint16_t length_ms = 1000;
        uint16_t sample_count = 1000;
    };

    /**
     * @brief Structure representing a single fluorometer sample.
     */
    struct FluorometerSample {
        float time_ms;
        uint16_t raw_value;
        float relative_value;
        float absolute_value;
    };

    /**
     * @brief Structure representing OJIP data from the fluorometer.
     */
    struct FluorometerOjipData {
        uint32_t measurement_id;
        Fluorometer_config::Gain detector_gain;
        Fluorometer_config::Timing timebase;
        float emitor_intensity;
        std::vector<FluorometerSample> samples;

        bool read;
        uint16_t length_ms;
        uint16_t required_samples;
        uint16_t captured_samples;
        int16_t missing_samples;
        bool saturated;
        std::string iso_start_time;
    };

    /**
     * @brief Structure representing detector info from the fluorometer.
     */
    struct FluorometerDetectorInfo {
        uint16_t peak_wavelength;
        uint16_t sensitivity;
        uint16_t sampling_rate;
    };

    /**
     * @brief Structure representing emitor info from the fluorometer.
     */
    struct FluorometerEmitorInfo {
        uint16_t peak_wavelength;
        uint16_t power_output;
    };

    /**
     * @brief Structure representing info from the spectrophotometer.
     */
    struct SpectroChannelInfo {
        uint8_t channel;
        uint16_t peak_wavelength;
        uint8_t half_intensity_peak_width;
    };

    /**
     * @brief Structure representing measurement of single channel of the spectrophotometer.
     */
    struct SpectroChannelMeasurement {
        uint8_t channel;
        float relative_value;
        uint16_t absolute_value;
    };

    virtual ModuleID id() const = 0;
    /**
     * @brief Retrieves the measured temperature from the top sensor of the bottle.
     */
    virtual std::future <float> getBottleTemperature() = 0;
    /**
     * @brief Retrieves the measured temperature from the top sensor of the bottle.
     */
    virtual std::future <float> getTopMeasuredTemperature() = 0;
    /**
     * @brief Retrieves the measured temperature from the bottom sensor of the bottle.
     */
    virtual std::future <float> getBottomMeasuredTemperature() = 0;
    /**
     * @brief Retrieves the temperature of the top sensor case of the bottle.
     */
    virtual std::future <float> getTopSensorTemperature() = 0;
    /**
     * @brief Retrieves the temperature of the bottom sensor case of the bottle.
     */
    virtual std::future <float> getBottomSensorTemperature() = 0;

    /**
     * @brief Clears custom text on Mini OLED display and displays the serial number.
     */
    virtual std::future <bool> clearCustomText() = 0;

    /**
     * @brief Prints custom text on the display of the module.
     * @param text The text to be displayed.
     */
    virtual std::future <bool> printCustomText(std::string text) = 0;

    /**
     * @brief Perform single sample measurement on fluorometer.
     */
    virtual std::future<FluorometerSample> takeFluorometerSingleSample(Fluorometer_config::Gain gain, float intensity) = 0;

    /**
     * @brief Starts OJIP capture on fluorometer
     */
    virtual std::future <FluorometerOjipData> captureFluorometerOjip(const FluorometerInput& input) = 0;

    /**
     * @brief Checks if fluorometer OJIP capture is complete.
     */
    virtual std::future <bool> isFluorometerOjipCaptureComplete() = 0;

    /**
     * @brief Retrieves OJIP data from the fluorometer.
     */
    virtual std::future<FluorometerOjipData> retrieveLastFluorometerOjipData() = 0;

    /**
     * @brief Retrieves information about the fluorometer detector.
     */
    virtual std::future <FluorometerDetectorInfo> getFluorometerDetectorInfo() = 0;

    /**
     * @brief Retrieves the temperature of the fluorometer detector.
     */
    virtual std::future <float> getFluorometerDetectorTemperature() = 0;

    /**
     * @brief Retrieves information about the fluorometer emitor.
     */
    virtual std::future <FluorometerEmitorInfo> getFluorometerEmitorInfo() = 0;

    /**
     * @brief Retrieves the temperature of the fluorometer emitor.
     */
    virtual std::future <float> getFluorometerEmitorTemperature() = 0;

    /**
     * @brief Request self-calibration of fluorometer.
     */
    virtual std::future <bool> calibrateFluorometer() = 0;

    /**
     * @brief Reads number of channels available on spectrophotometer.
     */
    virtual std::future <int8_t> getSpectrophotometerChannels() = 0;

    /**
     * @brief Read information about spectrophotometer channel.
     */
    virtual std::future <SpectroChannelInfo> getSpectrophotometerChannelInfo(int8_t channelNumber) = 0;

    /**
     * @brief Measure selected single channel and return response.
     */
    virtual std::future <SpectroChannelMeasurement> measureSpectrophotometerChannel(int8_t channelNumber) = 0;

    /**
     * @brief Retrieves the temperature of the spectrophotometer emitor.
     */
    virtual std::future <float> getSpectrophotometerEmitorTemperature() = 0;

    /**
     * @brief Request self-calibration of spectrophotometer.
     */
    virtual std::future <bool> calibrateSpectrophotometer() = 0;


    };


