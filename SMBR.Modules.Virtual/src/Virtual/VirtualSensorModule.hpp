#pragma once    

#include <SMBR/ISensorModule.hpp>

class VirtualSensorModule : public ISensorModule {
public:
    ModuleID id() const override {
        return {Sensor, 3};
    }

    std::future <float> getBottleTemperature() override;
    std::future <float> getTopMeasuredTemperature() override;
    std::future <float> getBottomMeasuredTemperature() override;
    std::future <float> getTopSensorTemperature() override;
    std::future <float> getBottomSensorTemperature() override;
    
};

