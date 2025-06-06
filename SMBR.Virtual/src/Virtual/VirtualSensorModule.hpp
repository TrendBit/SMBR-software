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
    std::future <FluorometerSample> takeFluorometerSingleSample(Fluorometer_config::Gain gain, float intensity) override;
    std::future <FluorometerOjipData> captureFluorometerOjip(const ISensorModule::FluorometerInput& input) override;
    std::future <bool> isFluorometerOjipCaptureComplete() override;
    std::future <FluorometerOjipData> retrieveLastFluorometerOjipData() override;
    std::future <FluorometerDetectorInfo> getFluorometerDetectorInfo() override;
    std::future <float> getFluorometerDetectorTemperature() override;
    std::future <FluorometerEmitorInfo> getFluorometerEmitorInfo() override;
    std::future <float> getFluorometerEmitorTemperature() override;
    std::future <bool> calibrateFluorometer() override;
    std::future <int8_t> getSpectrophotometerChannels() override;
    std::future <SpectroChannelInfo> getSpectrophotometerChannelInfo(int8_t channelNumber) override;
    std::future <SpectroChannelMeasurement> measureSpectrophotometerChannel(int8_t channelNumber) override;
    std::future <float> getSpectrophotometerEmitorTemperature() override;
    std::future <bool> calibrateSpectrophotometer() override;
    
};

