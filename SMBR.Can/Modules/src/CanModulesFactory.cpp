#include "SMBR/CanModulesFactory.hpp"
#include "CanSystemModule.hpp"

std::shared_ptr<ISystemModule> CanModulesFactory::create(){
    ICanChannel::Ptr channel;

    channel = std::make_shared<CanChannel>();
    
    return std::make_shared<CanSystemModule>(channel);
}