#pragma once    

#include <SMBR/ISystemModule.hpp>


class VirtualSystemModule : public ISystemModule {
public:

    VirtualSystemModule();

    std::future <AvailableModules> getAvailableModules() override;
    std::shared_ptr <ISensorModule> sensorModule() override;
    std::shared_ptr <IControlModule> controlModule() override;
    std::shared_ptr <ICoreModule> coreModule() override;
    std::shared_ptr <ICommonModule> commonModule(Modules module) override; 
private:
    std::shared_ptr <ISensorModule> sensor;
    std::shared_ptr <IControlModule> control;
    std::shared_ptr <ICoreModule> core;
    std::shared_ptr <ICommonModule> common;
};

