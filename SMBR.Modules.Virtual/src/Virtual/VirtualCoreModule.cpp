#include "VirtualCoreModule.hpp"
#include <iostream>       // std::cout
#include <future>         // std::async, std::future
#include "Random.hpp"


std::future <std::string> VirtualCoreModule::getShortID() {
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return std::string("123456");
    });
}

std::future <std::string> VirtualCoreModule::getIpAddress() {
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return std::string("127.0.0.1");
    });   
} 

std::future <std::string> VirtualCoreModule::getHostname() {
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return std::string("localhost");
    });
}

std::future <int64_t> VirtualCoreModule::getSerialNumber() {
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return int64_t(123456);
    });
}

std::future <ICoreModule::PowerSupplyType> VirtualCoreModule::getPowerSupplyType() {
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return ICoreModule::PowerSupplyType{true, true, true, true};
    });
}

std::future <float> VirtualCoreModule::getVoltage5V() {
    return std::async(std::launch::async, []() {
        Random::randomDelay();;
        return Random::nextFloat(4.9, 5.1);
    });
}

std::future <float> VirtualCoreModule::getVoltageVIN() {
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return Random::nextFloat(11.9, 12.1);
    });
}

std::future <float> VirtualCoreModule::getVoltagePoE() {
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return Random::nextFloat(11.9, 12.1);
    });
}

std::future <float> VirtualCoreModule::getCurrentConsumption() {
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return Random::nextFloat(8.0, 10.0);
    });
}

std::future <float> VirtualCoreModule::getPowerDraw() {
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return Random::nextFloat(80.0, 100.0);
    });
}






