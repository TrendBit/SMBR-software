#include "VirtualControlModule.hpp"
#include <iostream>       // std::cout
#include <future>         // std::async, std::future
#include "Random.hpp"

std::future <bool> VirtualControlModule::setIntensity(float intensity, int channel){
    return std::async(std::launch::async, [intensity, channel]() {
        Random::randomDelay(100);
        return true;
    });
}

std::future <float> VirtualControlModule::getIntensity(int channel){
    return std::async(std::launch::async, [channel]() {
        Random::randomDelay(100);
        return Random::nextFloat(0.0, 1.0);
    });
}

std::future <float> VirtualControlModule::getLedTemperature(){
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return Random::nextFloat(20.0, 30.0);
    });
}

std::future <bool> VirtualControlModule::setHeaterIntensity(float intensity){
    return std::async(std::launch::async, [intensity]() {
        Random::randomDelay();
        return true;
    });
}

std::future <float> VirtualControlModule::getHeaterIntensity(){
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return Random::nextFloat(-1.0, 1.0);
    });
}

std::future <bool> VirtualControlModule::setHeaterTargetTemperature(float targetTemperature){
    return std::async(std::launch::async, [targetTemperature]() {
        Random::randomDelay();
        return true;
    });
}

std::future <float> VirtualControlModule::getHeaterTargetTemperature(){
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return Random::nextFloat(20.0, 30.0);
    });
}

std::future <float> VirtualControlModule::getHeaterPlateTemperature(){
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return Random::nextFloat(20.0, 30.0);
    });
}

std::future <bool> VirtualControlModule::turnOffHeater(){
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return true;
    });
}

std::future <bool> VirtualControlModule::setCuvettePumpSpeed(float speed){
    return std::async(std::launch::async, [speed]() {
        Random::randomDelay();
        return true;
    });
}

std::future <float> VirtualControlModule::getCuvettePumpSpeed(){
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return Random::nextFloat(-1.0, 1.0);
    });
}

std::future <bool> VirtualControlModule::setCuvettePumpFlowrate(float flowrate){
    return std::async(std::launch::async, [flowrate]() {
        Random::randomDelay();
        return true;
    });
}

std::future <float> VirtualControlModule::getCuvettePumpFlowrate(){
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return Random::nextFloat(-1000.0, 1000.0);
    });
}

std::future <bool> VirtualControlModule::moveCuvettePump(float volume, float flowrate){
    return std::async(std::launch::async, [volume, flowrate]() {
        Random::randomDelay();
        return true;
    });
}

std::future <bool> VirtualControlModule::primeCuvettePump(){
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return true;
    });
}

std::future <bool> VirtualControlModule::purgeCuvettePump(){
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return true;
    });
}

std::future <bool> VirtualControlModule::stopCuvettePump(){
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return true;
    });
}

std::future <bool> VirtualControlModule::setAeratorSpeed(float speed){
    return std::async(std::launch::async, [speed]() {
        Random::randomDelay();
        return true;
    });
}

std::future <float> VirtualControlModule::getAeratorSpeed(){
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return Random::nextFloat(0.0, 1.0);
    });
}

std::future <bool> VirtualControlModule::setAeratorFlowrate(float flowrate){
    return std::async(std::launch::async, [flowrate]() {
        Random::randomDelay();
        return true;
    });
}

std::future <float> VirtualControlModule::getAeratorFlowrate(){
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return Random::nextFloat(10.0, 5000.0);
    });
}

std::future <bool> VirtualControlModule::moveAerator(float volume, float flowrate){
    return std::async(std::launch::async, [volume, flowrate]() {
        Random::randomDelay();
        return true;
    });
}

std::future <bool> VirtualControlModule::stopAerator(){
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return true;
    });
}

std::future <bool> VirtualControlModule::setMixerSpeed(float speed){
    return std::async(std::launch::async, [speed]() {
        Random::randomDelay();
        return true;
    });
}

std::future <float> VirtualControlModule::getMixerSpeed(){
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return Random::nextFloat(0.0, 1.0);
    });
}

std::future <bool> VirtualControlModule::setMixerRpm(float rpm){
    return std::async(std::launch::async, [rpm]() {
        Random::randomDelay();
        return true;
    });
}

std::future <float> VirtualControlModule::getMixerRpm(){
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return Random::nextFloat(0.0, 10000.0);
    });
}

std::future <bool> VirtualControlModule::stirMixer(float rpm, float time){
    return std::async(std::launch::async, [rpm, time]() {
        Random::randomDelay();
        return true;
    });
}

std::future <bool> VirtualControlModule::stopMixer(){
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return true;
    });
}

