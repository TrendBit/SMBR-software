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
        common[Modules::Core] = std::make_shared <ProxyCommonModule<Transform>>(Modules::Core, m->commonModule(Modules::Core));
        common[Modules::Control] = std::make_shared <ProxyCommonModule<Transform>>(Modules::Control, m->commonModule(Modules::Control));
        common[Modules::Sensor] = std::make_shared <ProxyCommonModule<Transform>>(Modules::Sensor, m->commonModule(Modules::Sensor));
    }
    ProxySystemModule(ISystemModule::Ptr m, Transform t) : m(m), t(t){
        sensor = std::make_shared <ProxySensorModule<Transform>>(m->sensorModule(), t);
        control = std::make_shared <ProxyControlModule<Transform>>(m->controlModule(), t);
        core = std::make_shared <ProxyCoreModule<Transform>>(m->coreModule(), t);
        common[Modules::Core] = std::make_shared <ProxyCommonModule<Transform>>(Modules::Core, m->commonModule(Modules::Core), t);
        common[Modules::Control] = std::make_shared <ProxyCommonModule<Transform>>(Modules::Control, m->commonModule(Modules::Control), t);
        common[Modules::Sensor] = std::make_shared <ProxyCommonModule<Transform>>(Modules::Sensor, m->commonModule(Modules::Sensor), t);
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
    std::shared_ptr <ICommonModule> commonModule(Modules module) override {
        return common[module];
    }
private:
    ISystemModule::Ptr m;
    Transform t;
    
    std::shared_ptr <ISensorModule> sensor;
    std::shared_ptr <IControlModule> control;
    std::shared_ptr <ICoreModule> core;
    std::map <Modules, std::shared_ptr <ICommonModule> > common;
};

