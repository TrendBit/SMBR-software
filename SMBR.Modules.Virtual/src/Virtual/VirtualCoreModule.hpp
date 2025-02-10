#pragma once    

#include <SMBR/ICoreModule.hpp>

class VirtualCoreModule : public ICoreModule {
public:
    ModuleID id() const override {
        return {Core, "0x01"};
    }
    std::future <std::string> getShortID() override;
    std::future <std::string> getIpAddress() override;
    std::future <std::string> getHostname() override;
    std::future <int64_t> getSerialNumber() override;
    std::future <PowerSupplyType> getPowerSupplyType() override;
    std::future <float> getVoltage5V() override;
    std::future <float> getVoltageVIN() override;
    std::future <float> getVoltagePoE() override;
    std::future <float> getCurrentConsumption() override;
    std::future <float> getPowerDraw() override;
};

