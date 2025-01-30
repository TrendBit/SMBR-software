#pragma once    

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <future>
#include "SMBR/SMBR.hpp"


class ICommonModule {
public:
    typedef std::shared_ptr <ICommonModule> Ptr;

    virtual ~ICommonModule() = default;

    virtual ModuleID id() const = 0;
    /**
     * @brief Sends a ping request
     */
    virtual std::future <float> ping() = 0;

    /**
     * @brief Retrieves the CPU/MCU load .
     */
    virtual std::future <float> getCoreLoad() = 0;

    /**
     * @brief Retrieves the core temperature.
     */
    virtual std::future <float> getCoreTemp() = 0;

    /**
     * @brief Retrieves the board temperature.
     */
    virtual std::future <float> getBoardTemp() = 0;

    /**
     * @brief Sends a request to restart.
     */
    virtual std::future <bool> restartModule() = 0;

    /**
     * @brief Sends a request to reboot into USB boot loader.
     */
    virtual std::future <bool> rebootModuleUsbBootloader() = 0;

    /**
     * @brief Sends a request to reboot into CAN bootloader mode.
     */
    virtual std::future <bool> rebootModuleCanBootloader() = 0;

};

