#pragma once    

#include <SMBR/ICommonModule.hpp>


template <class Transform>
class ProxyCommonModule : public ICommonModule {
public:

    ProxyCommonModule(ICommonModule::Ptr mm) : m(mm), mName(mm->id()) {

    }
    ProxyCommonModule(ICommonModule::Ptr mm, Transform t) : m(mm), mName(mm->id()), t(t) {}

    ModuleID id() const override {
        return mName;
    }

    std::future <float> ping() override {
        return t.template transform<float>(mName, "ping", m->ping());
    }
    std::future <float> getCoreLoad() override {
        return t.template transform<float>(mName, "getCoreLoad", m->getCoreLoad());
    }
    std::future <float> getCoreTemp() override {
        return t.template transform<float>(mName, "getCoreTemp", m->getCoreTemp());
    }
    std::future <float> getBoardTemp() override {
        return t.template transform<float>(mName, "getBoardTemp", m->getBoardTemp());
    }
    std::future <bool> restartModule() override {
        return t.template transform<bool>(mName, "restartModule", m->restartModule());
    }
    std::future <bool> rebootModuleUsbBootloader() override {
        return t.template transform<bool>(mName, "rebootModuleUsbBootloader", m->rebootModuleUsbBootloader());
    }
    std::future <bool> rebootModuleCanBootloader() override {
        return t.template transform<bool>(mName, "rebootModuleCanBootloader", m->rebootModuleCanBootloader());
    }
    
    private:
        ICommonModule::Ptr m;
        ModuleID mName;
        Transform t;
};
