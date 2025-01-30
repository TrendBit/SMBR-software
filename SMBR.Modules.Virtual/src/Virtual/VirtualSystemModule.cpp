#include "VirtualSystemModule.hpp"
#include "VirtualCoreModule.hpp"
#include "VirtualControlModule.hpp"
#include "VirtualSensorModule.hpp"
#include "VirtualCommonModule.hpp"
#include <iostream>       // std::cout
#include <future>         // std::async, std::future
#include "Random.hpp"

VirtualSystemModule::VirtualSystemModule(){
    core = std::make_shared<VirtualCoreModule>();
    control = std::make_shared<VirtualControlModule>();
    sensor = std::make_shared<VirtualSensorModule>();
    common = std::make_shared<VirtualCommonModule>();
}

std::future <ISystemModule::AvailableModules> VirtualSystemModule::getAvailableModules() {
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return ISystemModule::AvailableModules {Modules::Core, Modules::Control, Modules::Sensor};
    });
}

std::shared_ptr <ISensorModule> VirtualSystemModule::sensorModule() {
    return sensor;
}

std::shared_ptr <IControlModule> VirtualSystemModule::controlModule() {
    return control;
}

std::shared_ptr <ICoreModule> VirtualSystemModule::coreModule() {
    return core;
}

std::shared_ptr <ICommonModule> VirtualSystemModule::commonModule(Modules) {
    return common;
}



