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

    common[core->id()] = std::make_shared<VirtualCommonModule>(core->id());
    common[control->id()] = std::make_shared<VirtualCommonModule>(control->id());
    common[sensor->id()] = std::make_shared<VirtualCommonModule>(sensor->id());
}

std::future <ISystemModule::AvailableModules> VirtualSystemModule::getAvailableModules() {
    return std::async(std::launch::async, [&]() {
        Random::randomDelay();
        return ISystemModule::AvailableModules {
            core->id(), 
            control->id(), 
            sensor->id()
        };
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

std::shared_ptr <ICommonModule> VirtualSystemModule::commonModule(ModuleID m) {
    return common.at(m);
}

std::set <ModuleID> VirtualSystemModule::existing() {
    std::set <ModuleID>  ret;
    for (auto m : common){
        ret.insert(m.first);
    }
    return ret;
}



