#include "SMBR/VirtualModulesFactory.hpp"
#include "VirtualSystemModule.hpp"

std::shared_ptr<ISystemModule> VirtualModulesFactory::create(){
    return std::make_shared<VirtualSystemModule>();
}