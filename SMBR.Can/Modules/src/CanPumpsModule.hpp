#pragma once

#include "SMBR/IPumpsModule.hpp"
#include "BaseModule.hpp"
#include "can/CanChannel.hpp"

class CanPumpsModule : public IPumpsModule {
public:
    CanPumpsModule(ModuleID id, ICanChannel::Ptr channel);
    virtual ~CanPumpsModule() = default;
    
    ModuleID id() const override;
    std::future<uint8_t> getPumpCount() override;
    std::future<PumpInfo> getPumpInfo(uint8_t pump_index) override;

private:
    BaseModule base;
    ICanChannel::Ptr channel;
};
