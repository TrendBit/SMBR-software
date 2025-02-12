#pragma once    

#include <SMBR/IControlModule.hpp>

template <class Transform>
class ProxyControlModule : public IControlModule {
public:

    ProxyControlModule(IControlModule::Ptr mm) : mName(mm->id()), m(mm){}
    ProxyControlModule(IControlModule::Ptr mm, Transform tt) : mName(mm->id()), m(mm), t(tt) {}

    ModuleID id() const override {
        return mName;
    }

    std::future <bool> setIntensity(float intensity, int channel) override {
        return t.template transform<bool>(mName, "setIntensity (ch=" + std::to_string(channel) + " inten=" + std::to_string((int)intensity * 100) + "%)", m->setIntensity(intensity, channel));
    }
    std::future <bool> setIntensities(float i0, float i1, float i2, float i3) override {
        return t.template transform<bool>(mName, "setIntensities (i0=" + std::to_string((int)i0 * 100) + "% i1=" + std::to_string((int)i1 * 100) + "% i2=" + std::to_string((int)i2 * 100) + "% i3=" + std::to_string((int)i3 * 100) + "%)", m->setIntensities(i0, i1, i2, i3));
    }
    std::future <float> getIntensity(int channel) override {
        return t.template transform<float>(mName, "getIntensity (ch=" + std::to_string(channel) + ")", m->getIntensity(channel));
    }
    std::future <float> getLedTemperature() override {
        return t.template transform<float>(mName, "getLedTemperature", m->getLedTemperature());
    }
    std::future <bool> setHeaterIntensity(float intensity) override {
        return t.template transform<bool>(mName, "setHeaterIntensity (inten=" + std::to_string((int)intensity * 100) + "%)", m->setHeaterIntensity(intensity));
    }
    std::future <float> getHeaterIntensity() override {
        return t.template transform<float>(mName, "getHeaterIntensity", m->getHeaterIntensity());
    }
    std::future <bool> setHeaterTargetTemperature(float targetTemperature) override {
        return t.template transform<bool>(mName, "setHeaterTargetTemperature (temp=" + std::to_string((int)targetTemperature) + ")", m->setHeaterTargetTemperature(targetTemperature));
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
        return t.template transform<bool>(mName, "setCuvettePumpSpeed (speed=" + std::to_string((int)speed) + ")", m->setCuvettePumpSpeed(speed));
    }
    std::future <float> getCuvettePumpSpeed() override {
        return t.template transform<float>(mName, "getCuvettePumpSpeed", m->getCuvettePumpSpeed());
    }
    std::future <bool> setCuvettePumpFlowrate(float flowrate) override {
        return t.template transform<bool>(mName, "setCuvettePumpFlowrate (flow=" + std::to_string((int)flowrate) + ")", m->setCuvettePumpFlowrate(flowrate));
    }
    std::future <float> getCuvettePumpFlowrate() override {
        return t.template transform<float>(mName, "getCuvettePumpFlowrate", m->getCuvettePumpFlowrate());
    }
    std::future <bool> moveCuvettePump(float volume, float flowrate) override {
        return t.template transform<bool>(mName, "moveCuvettePump (vol=" + std::to_string((int)volume) + " flow=" + std::to_string((int)flowrate) + ")", m->moveCuvettePump(volume, flowrate));
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
        return t.template transform<bool>(mName, "setAeratorSpeed (speed=" + std::to_string((int)speed) + ")", m->setAeratorSpeed(speed));
    }
    std::future <float> getAeratorSpeed() override {
        return t.template transform<float>(mName, "getAeratorSpeed", m->getAeratorSpeed());
    }
    std::future <bool> setAeratorFlowrate(float flowrate) override {
        return t.template transform<bool>(mName, "setAeratorFlowrate (flow=" + std::to_string((int)flowrate) + ")", m->setAeratorFlowrate(flowrate));
    }
    std::future <float> getAeratorFlowrate() override {
        return t.template transform<float>(mName, "getAeratorFlowrate", m->getAeratorFlowrate());
    }
    std::future <bool> moveAerator(float volume, float flowrate) override {
        return t.template transform<bool>(mName, "moveAerator (vol=" + std::to_string((int)volume) + " flow=" + std::to_string((int)flowrate) + ")", m->moveAerator(volume, flowrate));
    }
    std::future <bool> stopAerator() override {
        return t.template transform<bool>(mName, "stopAerator", m->stopAerator());
    }
    std::future <bool> setMixerSpeed(float speed) override {
        return t.template transform<bool>(mName, "setMixerSpeed (speed=" + std::to_string((int)speed) + ")", m->setMixerSpeed(speed));
    }
    std::future <float> getMixerSpeed() override {
        return t.template transform<float>(mName, "getMixerSpeed", m->getMixerSpeed());
    }
    std::future <bool> setMixerRpm(float rpm) override {
        return t.template transform<bool>(mName, "setMixerRpm (rpm=" + std::to_string((int)rpm) + ")", m->setMixerRpm(rpm));
    }
    std::future <float> getMixerRpm() override {
        return t.template transform<float>(mName, "getMixerRpm", m->getMixerRpm());
    }
    std::future <bool> stirMixer(float rpm, float time) override {
        return t.template transform<bool>(mName, "stirMixer (rpm=" + std::to_string((int)rpm) + " time=" + std::to_string((int)time) + ")", m->stirMixer(rpm, time));
    }
    std::future <bool> stopMixer() override {
        return t.template transform<bool>(mName, "stopMixer", m->stopMixer());
    }


    private:
        ModuleID mName;
        IControlModule::Ptr m;
        Transform t;
};



