#include "CanSensorModule.hpp"
#include "messages/bottle_temperature/temperature_request.hpp"
#include "messages/bottle_temperature/temperature_response.hpp"
#include "messages/bottle_temperature/top_measured_temperature_request.hpp"
#include "messages/bottle_temperature/top_measured_temperature_response.hpp"
#include "messages/bottle_temperature/bottom_measured_temperature_request.hpp"
#include "messages/bottle_temperature/bottom_measured_temperature_response.hpp"
#include "messages/bottle_temperature/top_sensor_temperature_request.hpp"
#include "messages/bottle_temperature/top_sensor_temperature_response.hpp"
#include "messages/bottle_temperature/bottom_sensor_temperature_request.hpp"
#include "messages/bottle_temperature/bottom_sensor_temperature_response.hpp"
#include "codes.hpp"
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
