#pragma once    

#include <SMBR/ICoreModule.hpp> 
#include <SMBR/ISystemModule.hpp>

template <class Transform>
class ProxyCoreModule : public ICoreModule {
public:

    ProxyCoreModule(ICoreModule::Ptr m) : mName(Modules::Core), m(m) {}
    ProxyCoreModule(ICoreModule::Ptr m, Transform t) : mName(Modules::Core), m(m), t(t) {}

    std::future <std::string> getShortID() override {
        return t.template transform<std::string>(mName, "getShortID", m->getShortID());
    }
    std::future <std::string> getIpAddress() override {
        return t.template transform<std::string>(mName, "getIpAddress", m->getIpAddress());
    }
    std::future <std::string> getHostname() override {
        return t.template transform<std::string>(mName, "getHostname", m->getHostname());
    }
    std::future <int64_t> getSerialNumber() override {
        return t.template transform<int64_t>(mName, "getSerialNumber", m->getSerialNumber());
    }
    std::future <PowerSupplyType> getPowerSupplyType() override {
        return t.template transform<PowerSupplyType>(mName, "getPowerSupplyType", m->getPowerSupplyType());
    }
    std::future <float> getVoltage5V() override {
        return t.template transform<float>(mName, "getVoltage5V", m->getVoltage5V());
    }
    std::future <float> getVoltageVIN() override {
        return t.template transform<float>(mName, "getVoltageVIN", m->getVoltageVIN());
    }
    std::future <float> getVoltagePoE() override {
        return t.template transform<float>(mName, "getVoltagePoE", m->getVoltagePoE());
    }
    std::future <float> getCurrentConsumption() override {
        return t.template transform<float>(mName, "getCurrentConsumption", m->getCurrentConsumption());
    }
    std::future <float> getPowerDraw() override {
        return t.template transform<float>(mName, "getPowerDraw", m->getPowerDraw());
    }

    
    private:
        Modules mName;
        ICoreModule::Ptr m;
        Transform t;
};
