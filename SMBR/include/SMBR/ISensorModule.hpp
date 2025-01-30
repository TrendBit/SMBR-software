#pragma once    

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <future>


class ISensorModule {
public:
    typedef std::shared_ptr <ISensorModule> Ptr;
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
};
    