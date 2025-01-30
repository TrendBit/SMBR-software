#pragma once    

#include <SMBR/ISystemModule.hpp>

#include "ProxyCommonModule.hpp"
#include "ProxySensorModule.hpp"
#include "ProxyControlModule.hpp"
#include "ProxyCoreModule.hpp"
#include <map>

template <class Transform>
class ProxySystemModule : public ISystemModule {
public:

    ProxySystemModule(ISystemModule::Ptr m) : m(m){
        sensor = std::make_shared <ProxySensorModule<Transform>>(m->sensorModule());
        control = std::make_shared <ProxyControlModule<Transform>>(m->controlModule());
        core = std::make_shared <ProxyCoreModule<Transform>>(m->coreModule());
        
        common[core->id()] = std::make_shared <ProxyCommonModule<Transform>>(m->commonModule(core->id()));
        common[control->id()] = std::make_shared <ProxyCommonModule<Transform>>(m->commonModule(control->id()));;
        common[sensor->id()] = std::make_shared <ProxyCommonModule<Transform>>(m->commonModule(sensor->id()));
    }
    ProxySystemModule(ISystemModule::Ptr m, Transform t) : m(m), t(t){
        sensor = std::make_shared <ProxySensorModule<Transform>>(m->sensorModule(), t);
        control = std::make_shared <ProxyControlModule<Transform>>(m->controlModule(), t);
        core = std::make_shared <ProxyCoreModule<Transform>>(m->coreModule(), t);
        
        common[core->id()] = std::make_shared <ProxyCommonModule<Transform>>(m->commonModule(core->id()), t);
        common[control->id()] = std::make_shared <ProxyCommonModule<Transform>>(m->commonModule(control->id()), t);
        common[sensor->id()] = std::make_shared <ProxyCommonModule<Transform>>(m->commonModule(sensor->id()), t);
    }

    std::future <AvailableModules> getAvailableModules() override {
        return t.template transform<AvailableModules>(Modules::Core, "getAvailableModules", m->getAvailableModules());
    }
    std::shared_ptr <ISensorModule> sensorModule() override {
        return sensor;
    }
    std::shared_ptr <IControlModule> controlModule() override {
        return control;
    }
    std::shared_ptr <ICoreModule> coreModule() override {
        return core;
    }
    std::shared_ptr <ICommonModule> commonModule(ModuleID module) override {
        return common[module];
    }
private:
    ISystemModule::Ptr m;
    Transform t;
    
    std::shared_ptr <ISensorModule> sensor;
    std::shared_ptr <IControlModule> control;
    std::shared_ptr <ICoreModule> core;
    std::map <ModuleID, std::shared_ptr <ICommonModule> > common;
};

