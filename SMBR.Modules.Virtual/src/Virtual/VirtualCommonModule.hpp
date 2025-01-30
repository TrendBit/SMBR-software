#pragma once    

#include <SMBR/ICommonModule.hpp>



class VirtualCommonModule : public ICommonModule {
public:
    std::future <float> ping() override;
    std::future <float> getCoreLoad() override;
    std::future <float> getCoreTemp() override;
    std::future <float> getBoardTemp() override;
    std::future <bool> restartModule() override;
    std::future <bool> rebootModuleUsbBootloader() override;
    std::future <bool> rebootModuleCanBootloader() override;
};


