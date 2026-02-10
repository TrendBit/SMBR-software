#pragma once

#include <SMBR/IPumpsModule.hpp>

class VirtualPumpsModule : public IPumpsModule {
public:
    VirtualPumpsModule(Instance instance);
    
    ModuleID id() const override;
    std::future<uint8_t> getPumpCount() override;

private:
    ModuleID moduleId_;
};
