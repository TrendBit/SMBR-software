#pragma once

#include <SMBR/IPumpsModule.hpp>

class VirtualPumpsModule : public IPumpsModule {
public:
    VirtualPumpsModule(Instance instance);
    
    ModuleID id() const override;
    std::future<uint8_t> getPumpCount() override;
    std::future<PumpInfo> getPumpInfo(uint8_t pump_index) override;
    std::future<float> getSpeed(uint8_t pump_index) override;

private:
    ModuleID moduleId_;
};
