#pragma once    

#include <SMBR/ICommonModule.hpp>



class VirtualCommonModule : public ICommonModule {
public:
    VirtualCommonModule(ModuleID i) : id_(i){

    }

    ModuleID id() const override {
        return id_;
    }
    
    std::future <float> ping() override;
    std::future <float> getCoreLoad() override;
    std::future <float> getCoreTemp() override;
    std::future <float> getBoardTemp() override;
    std::future <bool> restartModule() override;
    std::future <bool> rebootModuleUsbBootloader() override;
    std::future <bool> rebootModuleCanBootloader() override;
private:
    ModuleID id_;
};


