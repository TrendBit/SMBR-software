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
     * @brief Structure representing a single fluorometer sample.
     */
    struct FluorometerSample {
        float time_ms;
        int16_t raw_value;
        float relative_value;
        float absolute_value;
    };

    /**
     * @brief Structure representing OJIP data from the fluorometer.
     */
    struct FluorometerOjipData {
        int32_t measurement_id;
        Fluorometer_config::Gain detector_gain;
        Fluorometer_config::Timing timebase;
        float emitor_intensity;
        std::vector<FluorometerSample> samples;

        bool read;
        int16_t length_ms;
        int16_t required_samples;
        int16_t captured_samples;
        int32_t missing_samples;
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
     * @brief Starts OJIP capture on fluorometer
     */
    virtual std::future <FluorometerOjipData> startFluorometerOjipCapture(
        Fluorometer_config::Gain detector_gain, 
        Fluorometer_config::Timing sample_timing, 
        float emitor_intensity, 
        uint16_t length_ms, 
        uint16_t samples) = 0;

    /**
     * @brief Checks if fluorometer OJIP capture is complete.
     */
    virtual std::future <bool> isFluorometerOjipCaptureComplete() = 0;

    /**
     * @brief Retrieves OJIP data from the fluorometer.
     */
    virtual std::future<FluorometerOjipData> retrieveFluorometerOjipData() = 0;

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


    };

    
    