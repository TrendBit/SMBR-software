#include "VirtualSystemModule.hpp"
#include "VirtualCoreModule.hpp"
#include "VirtualControlModule.hpp"
#include "VirtualSensorModule.hpp"
#include "VirtualPumpsModule.hpp"
#include "VirtualCommonModule.hpp"
#include <SMBR/Exceptions.hpp>
#include <iostream>       // std::cout
#include <future>         // std::async, std::future
#include "Random.hpp"

VirtualSystemModule::VirtualSystemModule(){
    core = std::make_shared<VirtualCoreModule>();
    control = std::make_shared<VirtualControlModule>();
    sensor = std::make_shared<VirtualSensorModule>();

    pumps[Instance::Instance_1] = std::make_shared<VirtualPumpsModule>(Instance::Instance_1);
    pumps[Instance::Instance_2] = std::make_shared<VirtualPumpsModule>(Instance::Instance_2);

    common[core->id()] = std::make_shared<VirtualCommonModule>(core->id());
    common[control->id()] = std::make_shared<VirtualCommonModule>(control->id());
    common[sensor->id()] = std::make_shared<VirtualCommonModule>(sensor->id());
    
    for (const auto& pump : pumps) {
        common[pump.second->id()] = std::make_shared<VirtualCommonModule>(pump.second->id());
    }
}

std::future <ISystemModule::AvailableModules> VirtualSystemModule::getAvailableModules() {
    return std::async(std::launch::async, [&]() {
        Random::randomDelay();
        ISystemModule::AvailableModules modules = {
            core->id(), 
            control->id(), 
            sensor->id()
        };
        
        for (const auto& pump : pumps) {
            modules.push_back(pump.second->id());
        }
        
        return modules;
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

std::shared_ptr <IPumpsModule> VirtualSystemModule::pumpsModule(Instance instance) {
    auto it = pumps.find(instance);
    if (it == pumps.end()) {
        throw NotFoundException("Pumps module with specified instance not available");
    }
    return it->second;
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

std::shared_ptr<IModuleIssues> VirtualSystemModule::issues() {
        return nullptr; 
    }


