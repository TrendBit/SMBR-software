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
};
