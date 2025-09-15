#include "SMBR/CanModulesFactory.hpp"
#include "CanSystemModule.hpp"
#include "ModuleIssues.hpp"

std::shared_ptr<ISystemModule> CanModulesFactory::create(){
    //ICanChannel::Ptr channel;

    auto channel = std::make_shared<CanChannel>();

    auto issues = std::make_shared<ModuleIssues>(channel);

    return std::make_shared<CanSystemModule>(channel, issues);
}