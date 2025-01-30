#include "VirtualCommonModule.hpp"
#include <iostream>       // std::cout
#include <future>         // std::async, std::future
#include "Random.hpp"

std::future <float> VirtualCommonModule::ping(){
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return Random::nextFloat(0.5, 1.0);
    });
}

std::future <float> VirtualCommonModule::getCoreLoad(){
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return Random::nextFloat(0.9, 1.0);
    });
}

std::future <float> VirtualCommonModule::getCoreTemp(){
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return Random::nextFloat(20.0, 30.0);
    });
}

std::future <float> VirtualCommonModule::getBoardTemp(){
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return Random::nextFloat(30.0, 35.0);
    });
}

std::future <bool> VirtualCommonModule::restartModule(){
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return true;
    });
}

std::future <bool> VirtualCommonModule::rebootModuleUsbBootloader(){
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return true;
    });
}

std::future <bool> VirtualCommonModule::rebootModuleCanBootloader(){
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return true;
    });
}