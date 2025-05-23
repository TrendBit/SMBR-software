#pragma once    

#include <SMBR/IControlModule.hpp>


class VirtualControlModule : public IControlModule {
public:
    ModuleID id() const override {
        return {Control, "0x02"};
    }
    std::future <bool> setIntensity(float intensity, int8_t channelNumber) override;
    std::future <bool> setIntensities(float i0, float i1, float i2, float i3) override;
    std::future <LedIntensityMeasurement> getIntensity(int8_t channelNumber) override;
    std::future <float> getLedTemperature() override;
    std::future <bool> setHeaterIntensity(float intensity) override;
    std::future <float> getHeaterIntensity() override;
    std::future <bool> setHeaterTargetTemperature(float targetTemperature) override;
    std::future <float> getHeaterTargetTemperature() override;
    std::future <float> getHeaterPlateTemperature() override;
    std::future <bool> turnOffHeater() override;
    std::future <bool> setCuvettePumpSpeed(float speed) override;
    std::future <float> getCuvettePumpSpeed() override;
    std::future <bool> setCuvettePumpFlowrate(float flowrate) override;
    std::future <float> getCuvettePumpFlowrate() override;
    std::future <bool> moveCuvettePump(float volume, float flowrate) override;
    std::future <bool> primeCuvettePump() override;
    std::future <bool> purgeCuvettePump() override;
    std::future <bool> stopCuvettePump() override;
    std::future <bool> setAeratorSpeed(float speed) override;
    std::future <float> getAeratorSpeed() override;
    std::future <bool> setAeratorFlowrate(float flowrate) override;
    std::future <float> getAeratorFlowrate() override;
    std::future <bool> moveAerator(float volume, float flowrate) override;
    std::future <bool> stopAerator() override;
    std::future <bool> setMixerSpeed(float speed) override;
    std::future <float> getMixerSpeed() override;
    std::future <bool> setMixerRpm(float rpm) override;
    std::future <float> getMixerRpm() override;
    std::future <bool> stirMixer(float rpm, float time) override;
    std::future <bool> stopMixer() override;

};


