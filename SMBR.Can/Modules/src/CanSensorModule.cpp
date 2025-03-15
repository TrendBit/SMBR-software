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
#include "codes/messages/fluorometer/ojip_completed_request.hpp"
#include "codes/messages/fluorometer/ojip_completed_response.hpp"
#include "codes/codes.hpp"
#include <iostream>       // std::cout
#include <future>         // std::async, std::future

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

std::future<bool> CanSensorModule::isFluorometerOjipCaptureComplete() {
    return base.get<
        App_messages::Fluorometer::OJIP_completed_request, 
        App_messages::Fluorometer::OJIP_completed_response, 
        bool
    >([](App_messages::Fluorometer::OJIP_completed_response response){
        return response.completed;
    }, 2000);
}
