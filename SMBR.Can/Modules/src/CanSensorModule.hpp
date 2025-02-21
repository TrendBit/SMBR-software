#pragma once    

#include <SMBR/ISensorModule.hpp>
#include "can/CanChannel.hpp"
#include "BaseModule.hpp"

class CanSensorModule : public ISensorModule {
public:
    CanSensorModule(std::string uidHex, ICanChannel::Ptr channel);

    ModuleID id() const override;

    std::future <float> getBottleTemperature() override;
    std::future <float> getTopMeasuredTemperature() override;
    std::future <float> getBottomMeasuredTemperature() override;
    std::future <float> getTopSensorTemperature() override;
    std::future <float> getBottomSensorTemperature() override;
    std::future <bool> clearCustomTextOnOled() override;

private:
    BaseModule base;
    
};

