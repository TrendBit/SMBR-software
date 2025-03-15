#pragma once    

#include <SMBR/ISensorModule.hpp>

class VirtualSensorModule : public ISensorModule {
public:
    ModuleID id() const override {
        return {Sensor, "0x03"};
    }

    std::future <float> getBottleTemperature() override;
    std::future <float> getTopMeasuredTemperature() override;
    std::future <float> getBottomMeasuredTemperature() override;
    std::future <float> getTopSensorTemperature() override;
    std::future <float> getBottomSensorTemperature() override;
    std::future <bool> clearCustomText() override;
    std::future <bool> printCustomText(std::string text) override;
    std::future <bool> isFluorometerOjipCaptureComplete() override;
    
};

