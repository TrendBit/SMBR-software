#pragma once    

#include <SMBR/ISensorModule.hpp>
#include <SMBR/ISystemModule.hpp>


template <class Transform>
class ProxySensorModule : public ISensorModule {
public:
    ProxySensorModule(ISensorModule::Ptr m) : mName(Modules::Sensor), m(m) {}
    ProxySensorModule(ISensorModule::Ptr m, Transform t) : mName(Modules::Sensor), m(m), t(t) {}

    std::future <float> getBottleTemperature() override {
        return t.template transform<float>(mName, "getBottleTemperature", m->getBottleTemperature());
    }
    std::future <float> getTopMeasuredTemperature() override {
        return t.template transform<float>(mName, "getTopMeasuredTemperature", m->getTopMeasuredTemperature());
    }
    std::future <float> getBottomMeasuredTemperature() override {
        return t.template transform<float>(mName, "getBottomMeasuredTemperature", m->getBottomMeasuredTemperature());
    }
    std::future <float> getTopSensorTemperature() override {
        return t.template transform<float>(mName, "getTopSensorTemperature", m->getTopSensorTemperature());
    }
    std::future <float> getBottomSensorTemperature() override {
        return t.template transform<float>(mName, "getBottomSensorTemperature", m->getBottomSensorTemperature());
    }
    
    private:
        Modules mName;
        ISensorModule::Ptr m;
        Transform t;
};

