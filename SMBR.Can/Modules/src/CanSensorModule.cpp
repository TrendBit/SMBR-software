#include "CanSensorModule.hpp"
#include "codes/messages/bottle_temperature/temperature_request.hpp"
#include "codes/messages/bottle_temperature/temperature_response.hpp"
#include "codes/messages/bottle_temperature/top_measured_temperature_request.hpp"
#include "codes/messages/bottle_temperature/top_measured_temperature_response.hpp"
#include "codes/messages/bottle_temperature/bottom_measured_temperature_request.hpp"
#include "codes/messages/bottle_temperature/bottom_measured_temperature_response.hpp"
#include "codes/messages/bottle_temperature/top_sensor_temperature_request.hpp"
#include "codes/messages/bottle_temperature/top_sensor_temperature_response.hpp"
#include "codes/messages/bottle_temperature/bottom_sensor_temperature_request.hpp"
#include "codes/messages/bottle_temperature/bottom_sensor_temperature_response.hpp"
#include "codes/messages/mini_oled/clear_custom_text.hpp"
#include "codes/messages/mini_oled/print_custom_text.hpp"
#include "codes/messages/fluorometer/ojip_capture_request.hpp"
#include "codes/messages/fluorometer/ojip_completed_request.hpp"
#include "codes/messages/fluorometer/ojip_completed_response.hpp"
#include "codes/messages/fluorometer/fluorometer_config.hpp"
#include "codes/codes.hpp"
#include <iostream>       // std::cout
#include <future>         // std::async, std::future
#include <fstream>
#include <iostream>

CanSensorModule::CanSensorModule(std::string uidHex, ICanChannel::Ptr channel) : base({Modules::Sensor, uidHex}, channel){

}

ModuleID CanSensorModule::id() const {
    return base.id();
}

std::future <float> CanSensorModule::getBottleTemperature() {

    return base.get<
        App_messages::Bottle_temperature::Temperature_request, 
        App_messages::Bottle_temperature::Temperature_response, 
        float
    >([](App_messages::Bottle_temperature::Temperature_response response){
        return response.temperature;
    }, 2000);
}

std::future <float> CanSensorModule::getTopMeasuredTemperature() {

    return base.get<
        App_messages::Bottle_temperature::Top_measured_temperature_request, 
        App_messages::Bottle_temperature::Top_measured_temperature_response, 
        float
    >([](App_messages::Bottle_temperature::Top_measured_temperature_response response){
        return response.temperature;
    }, 2000);
}

std::future <float> CanSensorModule::getBottomMeasuredTemperature() {

    return base.get<
        App_messages::Bottle_temperature::Bottom_measured_temperature_request, 
        App_messages::Bottle_temperature::Bottom_measured_temperature_response, 
        float
    >([](App_messages::Bottle_temperature::Bottom_measured_temperature_response response){
        return response.temperature;
    }, 2000);
}

std::future <float> CanSensorModule::getTopSensorTemperature() {

    return base.get<
        App_messages::Bottle_temperature::Top_sensor_temperature_request, 
        App_messages::Bottle_temperature::Top_sensor_temperature_response, 
        float
    >([](App_messages::Bottle_temperature::Top_sensor_temperature_response response){
        return response.temperature;
    }, 2000);
}

std::future <float> CanSensorModule::getBottomSensorTemperature() {

    return base.get<
        App_messages::Bottle_temperature::Bottom_sensor_temperature_request, 
        App_messages::Bottle_temperature::Bottom_sensor_temperature_response, 
        float
    >([](App_messages::Bottle_temperature::Bottom_sensor_temperature_response response){
        return response.temperature;
    }, 2000);
}

std::future <bool> CanSensorModule::clearCustomText() {
    App_messages::Mini_OLED::Clear_custom_text r;
    return base.set(r);
}

std::future <bool> CanSensorModule::printCustomText(std::string text) {
    std::vector<std::future<bool>> results;

    for (size_t i = 0; i < text.length(); i += App_messages::Mini_OLED::Print_custom_text::max_length) {
        std::string chunk = text.substr(i, App_messages::Mini_OLED::Print_custom_text::max_length);
        App_messages::Mini_OLED::Print_custom_text r(chunk);
        results.push_back(base.set(r));
    }

    return std::async(std::launch::async, [results = std::move(results)]() mutable {
        for (auto& res : results) {
            if (!res.get()) return false;
        }
        return true;
    });
}

uint8_t CanSensorModule::CalculateMeasurementID(uint32_t api_id) {
    return (api_id % 15) + 1;
}

std::future<bool> CanSensorModule::startFluorometerOjipCapture(
    Fluorometer_config::Gain detector_gain,
    Fluorometer_config::Timing sample_timing,
    float emitor_intensity,
    uint16_t length_ms,
    uint16_t samples) {

    uint32_t api_id;
    uint8_t measurement_id;

    std::fstream file("/home/reactor/measurement_params.txt", std::ios::in | std::ios::out); //for testing and for better readability this file is used
    if (file.is_open()) {
        uint32_t existing_api_id;
        if (file >> existing_api_id) {
            last_api_id = existing_api_id; 
            api_id = ++last_api_id; 
            measurement_id = CalculateMeasurementID(api_id); 
        } else {
            api_id = 1;
            measurement_id = CalculateMeasurementID(api_id);
            last_api_id = api_id;
        }

        last_measurement_data = ISensorModule::FluorometerOjipData{}; 
        isRead = false; 
        last_timebase = sample_timing; 

        file.seekp(0); 
        file << api_id << " " << samples << " " << length_ms << " "
             << static_cast<int>(sample_timing) << " " << isRead << "\n";
        file.close();
    } else {
        std::ofstream outfile("/home/reactor/measurement_params.txt");
        if (outfile.is_open()) {
            api_id = 1;
            measurement_id = CalculateMeasurementID(api_id);
            last_api_id = api_id;

            last_measurement_data = ISensorModule::FluorometerOjipData{};
            isRead = false; 
            last_timebase = sample_timing;

            outfile << api_id << " " << samples << " " << length_ms << " "
                    << static_cast<int>(sample_timing) << " " << isRead << "\n";
            outfile.close();
        } else {
            throw std::runtime_error("Failed to create measurement parameters file");
        }
    }

    last_required_samples = samples;
    last_length_ms = length_ms;

    App_messages::Fluorometer::OJIP_capture_request r{
        measurement_id,
        detector_gain,
        sample_timing,
        static_cast<uint8_t>(emitor_intensity * 255),
        length_ms,
        samples
    };

    return base.set(r);
}

std::future<bool> CanSensorModule::isFluorometerOjipCaptureComplete() {
    return base.get<
        App_messages::Fluorometer::OJIP_completed_request, 
        App_messages::Fluorometer::OJIP_completed_response, 
        bool
    >([](App_messages::Fluorometer::OJIP_completed_response response){
        return response.completed;
    }, 2000);
}
