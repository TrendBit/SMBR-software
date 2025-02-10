#pragma once    

#include <SMBR/ICommonModule.hpp>
#include "can/CanChannel.hpp"
#include "BaseModule.hpp"

class CanCommonModule : public ICommonModule {
public:
    CanCommonModule(ModuleID id, ICanChannel::Ptr channel);

    ModuleID id() const override;

    std::future <float> ping() override;
    std::future <float> getCoreLoad() override;
    std::future <float> getCoreTemp() override;
    std::future <float> getBoardTemp() override;
    std::future <bool> restartModule() override;
    std::future <bool> rebootModuleUsbBootloader() override;
    std::future <bool> rebootModuleCanBootloader() override;
   
private:
    BaseModule base;
   
};

