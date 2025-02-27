#pragma once    

#include <SMBR/ISystemModule.hpp>
#include "can/CanChannel.hpp"
#include "BaseModule.hpp"
#include <map>

class CanSystemModule : public ISystemModule {
public:
    CanSystemModule(ICanChannel::Ptr channel);

    std::future <AvailableModules> getAvailableModules() override;
    std::shared_ptr <ISensorModule> sensorModule() override;
    std::shared_ptr <IControlModule> controlModule() override;
    std::shared_ptr <ICoreModule> coreModule() override;
    std::shared_ptr <ICommonModule> commonModule(ModuleID module) override;

    std::set <ModuleID> existing() override;
private:
    
    std::mutex m;
    
    std::shared_ptr <ISensorModule> sensor;
    std::shared_ptr <IControlModule> control;
    std::shared_ptr <ICoreModule> core;
    std::map <ModuleID, std::shared_ptr <ICommonModule> > common;

    void refresh();

private:
    ICanChannel::Ptr channel;
    
};

