#pragma once    

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <future>
#include <ostream>

class ICoreModule {
public:
    typedef std::shared_ptr <ICoreModule> Ptr;

    virtual ~ICoreModule() = default;
    
    /**
     * @brief Retrieves the short ID (SID) of the device.
     */
    virtual std::future <std::string> getShortID() = 0;

    /**
     * @brief Retrieves the IP address of the device.
     */
    virtual std::future <std::string> getIpAddress() = 0;

    /**
     * @brief Retrieves the hostname of the device.
     */
    virtual std::future <std::string> getHostname() = 0;

    /**
     * @brief Retrieves the serial number of the device.
     */
    virtual std::future <int64_t> getSerialNumber() = 0;

    /**
     * @brief Retrieves the type of power supply powering the device.
     */
    struct PowerSupplyType {
        bool isSuccess; 
        bool isVIN;
        bool isPoE;
        bool isPoE_Hb;
    };
    virtual std::future <PowerSupplyType> getPowerSupplyType() = 0;

    /**
     * @brief Retrieves the voltage of the 5V power rail.
     */
    virtual std::future <float> getVoltage5V() = 0;

    /**
     * @brief Retrieves the voltage at the VIN power rail (12V).
     */
    virtual std::future <float> getVoltageVIN() = 0;

    /**
     * @brief Retrieves the voltage of PoE power rail (12V).
     */
    virtual std::future <float> getVoltagePoE() = 0;

    /**
     * @brief Retrieves the current consumption of the device.
     */
    virtual std::future <float> getCurrentConsumption() = 0;

    /**
     * @brief Retrieves the power draw of the device in watts.
     */
    virtual std::future <float> getPowerDraw() = 0;

};

std::ostream& operator<<(std::ostream& os, const ICoreModule::PowerSupplyType & v);

