#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <future>
#include "SMBR/SMBR.hpp"

class IControlModule {
public:
    typedef std::shared_ptr <IControlModule> Ptr;

    virtual ~IControlModule() = default;

    virtual ModuleID id() const = 0;

   /**
    * @brief Sets the intensity of LEDs on a module.
    */
    virtual std::future <bool> setIntensity(float intensity, int channel) = 0;

    /**
     * @brief Retrieves the current intensity of LEDs for a specified module and channel.
     */
    virtual std::future <float> getIntensity(int channel) = 0;

    /**
     * @brief Retrieves the current temperature of LEDs for a specified module and channel.
     */
    virtual std::future <float> getLedTemperature() = 0;

    /**
     * @brief Sets the intensity of heating or cooling.
     */
    virtual std::future <bool> setHeaterIntensity(float intensity) = 0;

    /**
     * @brief Retrieves the current intensity of heating or cooling.
     */
    virtual std::future <float> getHeaterIntensity() = 0;

    /**
     * @brief Sets the target temperature for the heater.
     */
    virtual std::future <bool> setHeaterTargetTemperature(float targetTemperature) = 0;

    /**
     * @brief Retrieves the currently set target temperature for the heater.
     */
    virtual std::future <float> getHeaterTargetTemperature() = 0;

    /**
     * @brief Retrieves the current temperature of the heater plate (metal heatspreader).
     */
    virtual std::future <float> getHeaterPlateTemperature() = 0;

    /**
     * @brief Turns off the heater by setting intensity to 0.0 and disabling temperature regulation.
     */
    virtual std::future <bool> turnOffHeater() = 0;

    /**
     * @brief Sets the speed of the cuvette pump.
     */
    virtual std::future <bool> setCuvettePumpSpeed(float speed) = 0;

    /**
     * @brief Retrieves the current speed of the cuvette pump.
     */
    virtual std::future <float> getCuvettePumpSpeed() = 0;

    /**
     * @brief Sets the flowrate of the cuvette pump.
     */
    virtual std::future <bool> setCuvettePumpFlowrate(float flowrate) = 0;

    /**
     * @brief Retrieves the current flowrate of the cuvette pump.
     */
    virtual std::future <float> getCuvettePumpFlowrate() = 0;

    /**
     * @brief Moves the requested amount of liquid in or out of the cuvette.
     */
    virtual std::future <bool> moveCuvettePump(float volume, float flowrate) = 0;

    /**
     * @brief Sends a command to prime the cuvette pump.
     */
    virtual std::future <bool> primeCuvettePump() = 0;

    /**
     * @brief Sends a command to purge the cuvette pump.
     */
    virtual std::future <bool> purgeCuvettePump() = 0;

    /**
     * @brief Sends a command to stop the cuvette pump and disable any planned movements.
     */
    virtual std::future <bool> stopCuvettePump() = 0;

    /**
     * @brief Sets the speed of the aerator.
     */
    virtual std::future <bool> setAeratorSpeed(float speed) = 0;

    /**
     * @brief Retrieves the current speed of the aerator.
     */
    virtual std::future <float> getAeratorSpeed() = 0;

    /**
     * @brief Sets the flowrate of the aerator.
     */
    virtual std::future <bool> setAeratorFlowrate(float flowrate) = 0;

    /**
     * @brief Retrieves the current flowrate of the aerator.
     */
    virtual std::future <float> getAeratorFlowrate() = 0;

    /**
     * @brief Moves the requested amount of air into the bottle using the aerator.
     */
    virtual std::future <bool> moveAerator(float volume, float flowrate) = 0;

    /**
     * @brief Sends a command to stop the aerator and disable any planned movements.
     */
    virtual std::future <bool> stopAerator() = 0;

    /**
     * @brief Sets the speed of the mixer.
     */
    virtual std::future <bool> setMixerSpeed(float speed) = 0;

    /**
     * @brief Retrieves the current speed of the mixer.
     */
    virtual std::future <float> getMixerSpeed() = 0;

    /**
     * @brief Sets the target RPM of the mixer.
     */
    virtual std::future <bool> setMixerRpm(float rpm) = 0;

    /**
     * @brief Retrieves the current RPM of the mixer.
     */
    virtual std::future <float> getMixerRpm() = 0;

    /**
     * @brief Sets the mixer to stir at a specified RPM for a specified time.
     */
    virtual std::future <bool> stirMixer(float rpm, float time) = 0;

    /**
     * @brief Stops the mixer immediately.
     */
    virtual std::future <bool> stopMixer() = 0; 
};



    