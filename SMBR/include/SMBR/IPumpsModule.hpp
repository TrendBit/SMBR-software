#pragma once

#include <memory>
#include <future>
#include "SMBR.hpp"

/**
 * @brief Interface for Pumps Module operations.
 * 
 */
class IPumpsModule {
public:
    typedef std::shared_ptr<IPumpsModule> Ptr;
    
    virtual ~IPumpsModule() = default;
    
    /**
     * @brief Structure representing pump information (flow rate limits).
     */
    struct PumpInfo {
        float max_flowrate;
        float min_flowrate;
    };
    
    /**
     * @brief Get the module ID.
     */
    virtual ModuleID id() const = 0;
    
    /**
     * @brief Retrieves the number of pumps available in this module instance.
     * 
     * @return A future that will contain the pump count (2-4).
     */
    virtual std::future<uint8_t> getPumpCount() = 0;
    
    /**
     * @brief Retrieves information about a specific pump.
     */
    virtual std::future<PumpInfo> getPumpInfo(uint8_t pump_index) = 0;
    
    /**
     * @brief Retrieves current speed of a specific pump.
     */
    virtual std::future<float> getSpeed(uint8_t pump_index) = 0;
    
    /**
     * @brief Sets speed of a specific pump.
     */
    virtual std::future<bool> setSpeed(uint8_t pump_index, float speed) = 0;
    
    /**
     * @brief Retrieves current flow rate of a specific pump.
     */
    virtual std::future<float> getFlowrate(uint8_t pump_index) = 0;
    
    /**
     * @brief Sets flow rate of a specific pump.
     */
    virtual std::future<bool> setFlowrate(uint8_t pump_index, float flowrate) = 0;
    
    /**
     * @brief Moves a specific volume of liquid with a specific pump.
     */
    virtual std::future<bool> move(uint8_t pump_index, float volume, float flowrate) = 0;
    
    /**
     * @brief Stops a specific pump.
     */
    virtual std::future<bool> stop(uint8_t pump_index) = 0;
};
