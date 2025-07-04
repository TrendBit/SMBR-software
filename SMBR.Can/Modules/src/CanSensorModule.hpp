#pragma once

#include <SMBR/ISensorModule.hpp>
#include "can/CanChannel.hpp"
#include "BaseModule.hpp"
#include "OjipMeasurementStorage.hpp"

#include "codes/messages/fluorometer/ojip_capture_request.hpp"
#include "codes/messages/fluorometer/ojip_completed_request.hpp"
#include "codes/messages/fluorometer/ojip_completed_response.hpp"
#include "codes/messages/fluorometer/ojip_retrieve_request.hpp"
#include "codes/messages/fluorometer/data_sample.hpp"
#include "codes/messages/fluorometer/fluorometer_config.hpp"



class CanSensorModule : public ISensorModule {
public:
    const unsigned int default_timeout_ms = 4000;

    CanSensorModule(std::string uidHex, ICanChannel::Ptr channel);

    ModuleID id() const override;

    std::future <float> getBottleTemperature() override;
    std::future <float> getTopMeasuredTemperature() override;
    std::future <float> getBottomMeasuredTemperature() override;
    std::future <float> getTopSensorTemperature() override;
    std::future <float> getBottomSensorTemperature() override;
    std::future <bool> clearCustomText() override;
    std::future <bool> printCustomText(std::string text) override;
    std::future <FluorometerSample> takeFluorometerSingleSample(Fluorometer_config::Gain gain, float intensity) override;
    std::future <FluorometerOjipData> captureFluorometerOjip(const FluorometerInput& input) override;
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

private:
    BaseModule base;
    ICanChannel::Ptr channel;
    std::atomic<uint32_t> last_api_id{0};
    std::atomic<uint32_t> sample_id{0};
    bool isRead = false;
    ISensorModule::FluorometerOjipData last_measurement_data;
    Fluorometer_config::Timing last_timebase = Fluorometer_config::Timing::Logarithmic;
    uint16_t last_required_samples;
    uint16_t last_length_ms;
    std::string last_iso_start_time;

    struct ProcessingContext {
        ISensorModule::FluorometerOjipData result;
        std::vector<ResponseData> responses;
        bool processed = false;
        int receivedSamples = 0;
        bool metadataLoaded = false;
        bool isSaturated = false;
        std::chrono::steady_clock::time_point firstSampleTime;
        std::chrono::steady_clock::time_point lastSampleTime;
        bool firstSampleLogged = false;
        std::chrono::steady_clock::time_point startTime;
    };

    void sendCanRequest(uint32_t timeoutMs, std::shared_ptr<std::promise<ISensorModule::FluorometerOjipData>> promise);
    void checkMeasurementCompletion(uint32_t timeoutMs, std::shared_ptr<std::promise<ISensorModule::FluorometerOjipData>> promise);
};

