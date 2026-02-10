#include "VirtualPumpsModule.hpp"
#include "Random.hpp"
#include <future>

VirtualPumpsModule::VirtualPumpsModule(Instance instance)
    : moduleId_(Modules::Pump, "0x04", instance) {
}

ModuleID VirtualPumpsModule::id() const {
    return moduleId_;
}

std::future<uint8_t> VirtualPumpsModule::getPumpCount() {
    return std::async(std::launch::async, [&]() {
        Random::randomDelay();
        return static_cast<uint8_t>(4);
    });
}

std::future<IPumpsModule::PumpInfo> VirtualPumpsModule::getPumpInfo(uint8_t pump_index) {
    return std::async(std::launch::async, [pump_index]() {
        Random::randomDelay();
        return IPumpsModule::PumpInfo{
            200.0f,  
            10.0f    
        };
    });
}

std::future<float> VirtualPumpsModule::getSpeed(uint8_t pump_index) {
    return std::async(std::launch::async, [pump_index]() {
        Random::randomDelay();
        return 0.5f;
    });
}

std::future<bool> VirtualPumpsModule::setSpeed(uint8_t pump_index, float speed) {
    return std::async(std::launch::async, [pump_index, speed]() {
        Random::randomDelay();
        return true;
    });
}

std::future<float> VirtualPumpsModule::getFlowrate(uint8_t pump_index) {
    return std::async(std::launch::async, [pump_index]() {
        Random::randomDelay();
        return 100.0f;
    });
}

std::future<bool> VirtualPumpsModule::setFlowrate(uint8_t pump_index, float flowrate) {
    return std::async(std::launch::async, [pump_index, flowrate]() {
        Random::randomDelay();
        return true;
    });
}
