#pragma once    

#include <SMBR/ICoreModule.hpp>
#include "can/CanChannel.hpp"
#include "BaseModule.hpp"

class CanCoreModule : public ICoreModule {
public:
    CanCoreModule(std::string uidHex, ICanChannel::Ptr channel);

    ModuleID id() const override;

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

private:
    BaseModule base;
    
};

