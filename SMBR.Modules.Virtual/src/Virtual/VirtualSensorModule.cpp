#include "VirtualSensorModule.hpp"
#include <iostream>       // std::cout
#include <future>         // std::async, std::future
#include "Random.hpp"


std::future <float> VirtualSensorModule::getBottleTemperature() {
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return Random::nextFloat(20.0, 30.0);
    });
}

std::future <float> VirtualSensorModule::getTopMeasuredTemperature() {
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return Random::nextFloat(20.0, 30.0);
    });
}

std::future <float> VirtualSensorModule::getBottomMeasuredTemperature() {
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return Random::nextFloat(20.0, 30.0);
    });
}

std::future <float> VirtualSensorModule::getTopSensorTemperature() {
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return Random::nextFloat(20.0, 30.0);
    });
}

std::future <float> VirtualSensorModule::getBottomSensorTemperature() {
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return Random::nextFloat(20.0, 30.0);
    });
}







