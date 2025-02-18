#include "CanControlModule.hpp"
#include "codes/codes.hpp"
#include <iostream>       // std::cout
#include <future>         // std::async, std::future


#include "codes/messages/led_panel/set_intensity.hpp"
#include "codes/messages/led_panel/get_intensity_request.hpp"
#include "codes/messages/led_panel/get_intensity_response.hpp"
#include "codes/messages/led_panel/temperature_request.hpp"
#include "codes/messages/led_panel/temperature_response.hpp"
#include "codes/messages/heater/get_target_temperature_request.hpp"
#include "codes/messages/heater/get_target_temperature_response.hpp"
#include "codes/messages/heater/get_intensity_request.hpp"
#include "codes/messages/heater/get_intensity_response.hpp"
#include "codes/messages/heater/get_plate_temperature_request.hpp"
#include "codes/messages/heater/get_plate_temperature_response.hpp"
#include "codes/messages/heater/set_intensity.hpp"
#include "codes/messages/heater/set_target_temperature.hpp"
#include "codes/messages/heater/turn_off.hpp"
#include "codes/messages/cuvette_pump/set_speed.hpp"
#include "codes/messages/cuvette_pump/get_speed_request.hpp"
#include "codes/messages/cuvette_pump/get_speed_response.hpp"
#include "codes/messages/cuvette_pump/set_flowrate.hpp"
#include "codes/messages/cuvette_pump/get_flowrate_request.hpp"
#include "codes/messages/cuvette_pump/get_flowrate_response.hpp"
#include "codes/messages/cuvette_pump/move.hpp"
#include "codes/messages/cuvette_pump/prime.hpp"
#include "codes/messages/cuvette_pump/purge.hpp"
#include "codes/messages/cuvette_pump/stop.hpp"
#include "codes/messages/aerator/set_speed.hpp"
#include "codes/messages/aerator/get_speed_request.hpp"
#include "codes/messages/aerator/get_speed_response.hpp"
#include "codes/messages/aerator/set_flowrate.hpp"
#include "codes/messages/aerator/get_flowrate_request.hpp"
#include "codes/messages/aerator/get_flowrate_response.hpp"
#include "codes/messages/aerator/move.hpp"
#include "codes/messages/aerator/stop.hpp"
#include "codes/messages/mixer/set_speed.hpp"
#include "codes/messages/mixer/get_speed_request.hpp"
#include "codes/messages/mixer/get_speed_response.hpp"
#include "codes/messages/mixer/set_rpm.hpp"
#include "codes/messages/mixer/get_rpm_request.hpp"
#include "codes/messages/mixer/get_rpm_response.hpp"
#include "codes/messages/mixer/stir.hpp"
#include "codes/messages/mixer/stop.hpp"
#include "codes/messages/mini_oled/print_custom_text.hpp"
#include "codes/messages/mini_oled/clear_custom_text.hpp"


CanControlModule::CanControlModule(std::string uidHex, ICanChannel::Ptr channel) : base({Modules::Control, uidHex}, channel){

}

ModuleID CanControlModule::id() const {
    return base.id();
}

std::future <bool> CanControlModule::setIntensity(float intensity, int channel) {
    
    App_messages::LED_panel::Set_intensity r((uint8_t)channel, (float)intensity);
    return base.set(r);
}

std::future <bool> CanControlModule::setIntensities(float i0, float i1, float i2, float i3) {
    
    std::vector <App_messages::LED_panel::Set_intensity> requests;
    requests.push_back(App_messages::LED_panel::Set_intensity(0, i0));
    requests.push_back(App_messages::LED_panel::Set_intensity(1, i1));
    requests.push_back(App_messages::LED_panel::Set_intensity(2, i2));
    requests.push_back(App_messages::LED_panel::Set_intensity(3, i3));
        
    std::shared_ptr <std::promise <bool>> promise = std::make_shared<std::promise <bool>>();
    int successCount = 0;
    int failCount = 0;

    std::shared_ptr <std::pair <int, int>> counters = std::make_shared<std::pair <int, int>>(0, 0);
    for (auto & request : requests) {
        auto requestId = base.createRequestId(request.Type(), Codes::Instance::Exclusive, false);
        RequestData requestData(requestId, request.Export_data());
        CanRequest canRequest(requestData);
        base.ch()->send(canRequest, [&, promise, counters](ICanChannel::Response response){
            if (response.status == CanRequestStatus::Success) {
                counters->first++;
            } else {
                counters->second++;
            }
            if (counters->first + counters->second == 4) {
                if (counters->second > 0) {
                    promise->set_exception(std::make_exception_ptr(std::runtime_error("Failed to send request")));
                } else {
                    promise->set_value(true);
                }
            }
        });
    }
    
    return promise->get_future();
}

std::future <float> CanControlModule::getIntensity(int channel) {

    App_messages::LED_panel::Get_intensity_request r((uint8_t)channel);
    return base.get<
        App_messages::LED_panel::Get_intensity_request, 
        App_messages::LED_panel::Get_intensity_response, 
        float
    >(r, [](App_messages::LED_panel::Get_intensity_response response){
        return response.intensity;
    }, 2000);
}

std::future <float> CanControlModule::getLedTemperature() {
    App_messages::LED_panel::Temperature_request req;
    App_messages::LED_panel::Temperature_response resp{0};

    return base.get<
        App_messages::LED_panel::Temperature_request, 
        App_messages::LED_panel::Temperature_response, 
        float
    >(req, resp, [](App_messages::LED_panel::Temperature_response response){
        return response.temperature;
    }, 2000);
}

std::future <bool> CanControlModule::setHeaterIntensity(float intensity) {

    App_messages::Heater::Set_intensity r((float)intensity);
    return base.set(r);
}

std::future <float> CanControlModule::getHeaterIntensity() {
    return base.get<
        App_messages::Heater::Get_intensity_request, 
        App_messages::Heater::Get_intensity_response, 
        float
    >([](App_messages::Heater::Get_intensity_response response){
        return response.intensity;
    }, 2000);
}

std::future <bool> CanControlModule::setHeaterTargetTemperature(float targetTemperature) {

    App_messages::Heater::Set_target_temperature r((float)targetTemperature);
    return base.set(r);
}

std::future <float> CanControlModule::getHeaterTargetTemperature() {
    return base.get<
        App_messages::Heater::Get_target_temperature_request, 
        App_messages::Heater::Get_target_temperature_response, 
        float
    >([](App_messages::Heater::Get_target_temperature_response response){
        return response.temperature;
    }, 2000);
}

std::future <float> CanControlModule::getHeaterPlateTemperature() {
    return base.get<
        App_messages::Heater::Get_plate_temperature_request, 
        App_messages::Heater::Get_plate_temperature_response, 
        float
    >([](App_messages::Heater::Get_plate_temperature_response response){
        return response.temperature;
    }, 2000);
}

std::future <bool> CanControlModule::turnOffHeater() {

    App_messages::Heater::Heater_turn_off r;
    return base.set(r);
}

std::future <bool> CanControlModule::setCuvettePumpSpeed(float speed) {

    App_messages::Cuvette_pump::Set_speed r((float)speed);
    return base.set(r);
}

std::future <float> CanControlModule::getCuvettePumpSpeed() {
    return base.get<
        App_messages::Cuvette_pump::Get_speed_request, 
        App_messages::Cuvette_pump::Get_speed_response, 
        float
    >([](App_messages::Cuvette_pump::Get_speed_response response){
        return response.speed;
    }, 2000);
}

std::future <bool> CanControlModule::setCuvettePumpFlowrate(float flowrate) {

    App_messages::Cuvette_pump::Set_flowrate r((float)flowrate);
    return base.set(r);
}

std::future <float> CanControlModule::getCuvettePumpFlowrate() {
    return base.get<
        App_messages::Cuvette_pump::Get_flowrate_request, 
        App_messages::Cuvette_pump::Get_flowrate_response, 
        float
    >([](App_messages::Cuvette_pump::Get_flowrate_response response){
        return response.flowrate;
    }, 2000);
}

std::future <bool> CanControlModule::moveCuvettePump(float volume, float flowrate) {

    App_messages::Cuvette_pump::Move r((float)volume, (float)flowrate);
    return base.set(r);
}

std::future <bool> CanControlModule::primeCuvettePump() {

    App_messages::Cuvette_pump::Prime r;
    return base.set(r);
}

std::future <bool> CanControlModule::purgeCuvettePump() {

    App_messages::Cuvette_pump::Purge r;
    return base.set(r);
}

std::future <bool> CanControlModule::stopCuvettePump() {

    App_messages::Cuvette_pump::Stop r;
    return base.set(r);
}

std::future <bool> CanControlModule::setAeratorSpeed(float speed) {

    App_messages::Aerator::Set_speed r((float)speed);
    return base.set(r);
}

std::future <float> CanControlModule::getAeratorSpeed() {
    return base.get<
        App_messages::Aerator::Get_speed_request, 
        App_messages::Aerator::Get_speed_response, 
        float
    >([](App_messages::Aerator::Get_speed_response response){
        return response.speed;
    }, 2000);
}

std::future <bool> CanControlModule::setAeratorFlowrate(float flowrate) {

    App_messages::Aerator::Set_flowrate r((float)flowrate);
    return base.set(r);
}

std::future <float> CanControlModule::getAeratorFlowrate() {
    return base.get<
        App_messages::Aerator::Get_flowrate_request, 
        App_messages::Aerator::Get_flowrate_response, 
        float
    >([](App_messages::Aerator::Get_flowrate_response response){
        return response.flowrate;
    }, 2000);
}

std::future <bool> CanControlModule::moveAerator(float volume, float flowrate) {

    App_messages::Aerator::Move r((float)volume, (float)flowrate);
    return base.set(r);
}

std::future <bool> CanControlModule::stopAerator() {

    App_messages::Aerator::Stop r;
    return base.set(r);
}

std::future <bool> CanControlModule::setMixerSpeed(float speed) {

    App_messages::Mixer::Set_speed r((float)speed);
    return base.set(r);
}

std::future <float> CanControlModule::getMixerSpeed() {
    return base.get<
        App_messages::Mixer::Get_speed_request, 
        App_messages::Mixer::Get_speed_response, 
        float
    >([](App_messages::Mixer::Get_speed_response response){
        return response.speed;
    }, 2000);
}

std::future <bool> CanControlModule::setMixerRpm(float rpm) {

    App_messages::Mixer::Set_rpm r((float)rpm);
    return base.set(r);
}

std::future <float> CanControlModule::getMixerRpm() {
    return base.get<
        App_messages::Mixer::Get_rpm_request, 
        App_messages::Mixer::Get_rpm_response, 
        float
    >([](App_messages::Mixer::Get_rpm_response response){
        return response.rpm;
    }, 2000);
}

std::future <bool> CanControlModule::stirMixer(float rpm, float time) {

    App_messages::Mixer::Stir r((float)rpm, (float)time);
    return base.set(r);
}

std::future <bool> CanControlModule::stopMixer() {

    App_messages::Mixer::Stop r;
    return base.set(r);
}

std::future <bool> CanControlModule::printCustomText(std::string text) {
    App_messages::Mini_OLED::Print_custom_text r(text);
    return base.set(r);
}

std::future <bool> CanControlModule::clearCustomText() {
    App_messages::Mini_OLED::Clear_custom_text r;
    return base.set(r);
}


