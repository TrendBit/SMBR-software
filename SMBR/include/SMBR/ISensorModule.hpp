#pragma once    

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <future>
#include "SMBR/SMBR.hpp"

class ISensorModule {
public:
    typedef std::shared_ptr <ISensorModule> Ptr;

    virtual ~ISensorModule() = default;

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
     * @brief Checks if fluorometer OJIP capture is complete.
     */
    virtual std::future <bool> isFluorometerOjipCaptureComplete() = 0;
};
    