#pragma once    

#include <SMBR/IControlModule.hpp>

template <class Transform>
class ProxyControlModule : public IControlModule {
public:

    ProxyControlModule(IControlModule::Ptr m) : mName(Modules::Control), m(m){}
    ProxyControlModule(IControlModule::Ptr m, Transform t) : mName(Modules::Control), m(m), t(t) {}

    std::future <bool> setIntensity(float intensity, int channel) override {
        return t.template transform<bool>(mName, "setIntensity", m->setIntensity(intensity, channel));
    }
    std::future <float> getIntensity(int channel) override {
        return t.template transform<float>(mName, "getIntensity", m->getIntensity(channel));
    }
    std::future <float> getLedTemperature() override {
        return t.template transform<float>(mName, "getLedTemperature", m->getLedTemperature());
    }
    std::future <bool> setHeaterIntensity(float intensity) override {
        return t.template transform<bool>(mName, "setHeaterIntensity", m->setHeaterIntensity(intensity));
    }
    std::future <float> getHeaterIntensity() override {
        return t.template transform<float>(mName, "getHeaterIntensity", m->getHeaterIntensity());
    }
    std::future <bool> setHeaterTargetTemperature(float targetTemperature) override {
        return t.template transform<bool>(mName, "setHeaterTargetTemperature", m->setHeaterTargetTemperature(targetTemperature));
    }
    std::future <float> getHeaterTargetTemperature() override {
        return t.template transform<float>(mName, "getHeaterTargetTemperature", m->getHeaterTargetTemperature());
    }
    std::future <float> getHeaterPlateTemperature() override {
        return t.template transform<float>(mName, "getHeaterPlateTemperature", m->getHeaterPlateTemperature());
    }
    std::future <bool> turnOffHeater() override {
        return t.template transform<bool>(mName, "turnOffHeater", m->turnOffHeater());
    }
    std::future <bool> setCuvettePumpSpeed(float speed) override {
        return t.template transform<bool>(mName, "setCuvettePumpSpeed", m->setCuvettePumpSpeed(speed));
    }
    std::future <float> getCuvettePumpSpeed() override {
        return t.template transform<float>(mName, "getCuvettePumpSpeed", m->getCuvettePumpSpeed());
    }
    std::future <bool> setCuvettePumpFlowrate(float flowrate) override {
        return t.template transform<bool>(mName, "setCuvettePumpFlowrate", m->setCuvettePumpFlowrate(flowrate));
    }
    std::future <float> getCuvettePumpFlowrate() override {
        return t.template transform<float>(mName, "getCuvettePumpFlowrate", m->getCuvettePumpFlowrate());
    }
    std::future <bool> moveCuvettePump(float volume, float flowrate) override {
        return t.template transform<bool>(mName, "moveCuvettePump", m->moveCuvettePump(volume, flowrate));
    }
    std::future <bool> primeCuvettePump() override {
        return t.template transform<bool>(mName, "primeCuvettePump", m->primeCuvettePump());
    }
    std::future <bool> purgeCuvettePump() override {
        return t.template transform<bool>(mName, "purgeCuvettePump", m->purgeCuvettePump());
    }
    std::future <bool> stopCuvettePump() override {
        return t.template transform<bool>(mName, "stopCuvettePump", m->stopCuvettePump());
    }
    std::future <bool> setAeratorSpeed(float speed) override {
        return t.template transform<bool>(mName, "setAeratorSpeed", m->setAeratorSpeed(speed));
    }
    std::future <float> getAeratorSpeed() override {
        return t.template transform<float>(mName, "getAeratorSpeed", m->getAeratorSpeed());
    }
    std::future <bool> setAeratorFlowrate(float flowrate) override {
        return t.template transform<bool>(mName, "setAeratorFlowrate", m->setAeratorFlowrate(flowrate));
    }
    std::future <float> getAeratorFlowrate() override {
        return t.template transform<float>(mName, "getAeratorFlowrate", m->getAeratorFlowrate());
    }
    std::future <bool> moveAerator(float volume, float flowrate) override {
        return t.template transform<bool>(mName, "moveAerator", m->moveAerator(volume, flowrate));
    }
    std::future <bool> stopAerator() override {
        return t.template transform<bool>(mName, "stopAerator", m->stopAerator());
    }
    std::future <bool> setMixerSpeed(float speed) override {
        return t.template transform<bool>(mName, "setMixerSpeed", m->setMixerSpeed(speed));
    }
    std::future <float> getMixerSpeed() override {
        return t.template transform<float>(mName, "getMixerSpeed", m->getMixerSpeed());
    }
    std::future <bool> setMixerRpm(float rpm) override {
        return t.template transform<bool>(mName, "setMixerRpm", m->setMixerRpm(rpm));
    }
    std::future <float> getMixerRpm() override {
        return t.template transform<float>(mName, "getMixerRpm", m->getMixerRpm());
    }
    std::future <bool> stirMixer(float rpm, float time) override {
        return t.template transform<bool>(mName, "stirMixer", m->stirMixer(rpm, time));
    }
    std::future <bool> stopMixer() override {
        return t.template transform<bool>(mName, "stopMixer", m->stopMixer());
    }


    private:
        Modules mName;
        IControlModule::Ptr m;
        Transform t;
};



