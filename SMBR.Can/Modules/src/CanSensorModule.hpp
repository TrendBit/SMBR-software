#pragma once    

#include <SMBR/ISensorModule.hpp>
#include "can/CanChannel.hpp"
#include "BaseModule.hpp"

#include "codes/messages/fluorometer/ojip_capture_request.hpp"
#include "codes/messages/fluorometer/ojip_completed_request.hpp"
#include "codes/messages/fluorometer/ojip_completed_response.hpp"
#include "codes/messages/fluorometer/ojip_retrieve_request.hpp"
#include "codes/messages/fluorometer/data_sample.hpp"
#include "codes/messages/fluorometer/fluorometer_config.hpp"

struct MeasurementParams {
        uint32_t api_id;
        uint16_t samples;
        uint16_t length_ms;
        int timebase;
        bool isRead;
    };

class CanSensorModule : public ISensorModule {
public:
    CanSensorModule(std::string uidHex, ICanChannel::Ptr channel);

    ModuleID id() const override;

    std::future <float> getBottleTemperature() override;
    std::future <float> getTopMeasuredTemperature() override;
    std::future <float> getBottomMeasuredTemperature() override;
    std::future <float> getTopSensorTemperature() override;
    std::future <float> getBottomSensorTemperature() override;   
    std::future <bool> clearCustomText() override;
    std::future <bool> printCustomText(std::string text) override;
    std::future <FluorometerOjipData> startFluorometerOjipCapture(
        Fluorometer_config::Gain detector_gain, 
        Fluorometer_config::Timing sample_timing, 
        float emitor_intensity, 
        uint16_t length_ms, 
        uint16_t samples) override;
    std::future <bool> isFluorometerOjipCaptureComplete() override;
    std::future <FluorometerOjipData> retrieveFluorometerOjipData() override;
    

private:
    BaseModule base;
    ICanChannel::Ptr channel;
    uint8_t CalculateMeasurementID(uint32_t api_id);
    std::atomic<uint32_t> last_api_id{0}; 
    bool isRead = false;
    ISensorModule::FluorometerOjipData last_measurement_data; 
    Fluorometer_config::Timing last_timebase = Fluorometer_config::Timing::Logarithmic; 
    uint16_t last_required_samples;
    uint16_t last_length_ms;

    void sendCanRequest(uint32_t timeoutMs, std::shared_ptr<std::promise<ISensorModule::FluorometerOjipData>> promise);
    void checkMeasurementCompletion(uint32_t timeoutMs, std::shared_ptr<std::promise<ISensorModule::FluorometerOjipData>> promise);
    bool readMeasurementParams(const std::string& filePath, MeasurementParams& params);
    bool writeMeasurementParams(const std::string& filePath, const MeasurementParams& params);

    
};

