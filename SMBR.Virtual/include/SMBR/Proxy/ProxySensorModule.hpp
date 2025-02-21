#pragma once    

#include <SMBR/ISensorModule.hpp>
#include <SMBR/ISystemModule.hpp>


template <class Transform>
class ProxySensorModule : public ISensorModule {
public:
    ProxySensorModule(ISensorModule::Ptr mm) : mName(mm->id()), m(mm) {}
    ProxySensorModule(ISensorModule::Ptr mm, Transform t) : mName(mm->id()), m(mm), t(t){} 

    ModuleID id() const override {
        return mName;
    }

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
    std::future <bool> clearCustomText() override {
        return t.template transform<bool>(mName, "clearCustomText", m->clearCustomText());
    }
    std::future <bool> printCustomText(std::string text) override {
        return t.template transform<bool>(mName, "printCustomText (text=" + text + ")", m->printCustomText(text));
    }
    
    private:
        ModuleID mName;
        ISensorModule::Ptr m;
        Transform t;
};

