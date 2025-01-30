#pragma once    

#include <SMBR/ICommonModule.hpp>


template <class Transform>
class ProxyCommonModule : public ICommonModule {
public:

    ProxyCommonModule(Modules mName, ICommonModule::Ptr m) : mName(mName), m(m) {}
    ProxyCommonModule(Modules mName, ICommonModule::Ptr m, Transform t) : mName(mName), m(m), t(t) {}

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
        Modules mName;
        ICommonModule::Ptr m;
        Transform t;
};
