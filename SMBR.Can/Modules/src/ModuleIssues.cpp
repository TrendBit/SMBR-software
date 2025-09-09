#include "ModuleIssues.hpp"
#include "SMBR/Log.hpp"
#include <sstream>

ModuleIssues::ModuleIssues(std::shared_ptr<CanChannel> channel)
    : channel_(channel)
{
    channel_->setReceiveHandler([this](const CanChannel::ResponseData& response){
        handleIssue(response);
    });
}

void ModuleIssues::handleIssue(const CanChannel::ResponseData& response) {
    CanID canid(response.id);

    std::ostringstream oss;
    oss << "Module issue from module "
        << static_cast<int>(canid.moduleAddress())
        << " instance " << static_cast<int>(canid.instance())
        << " data:";
    for (auto b : response.data) {
        oss << " " << std::hex << static_cast<int>(b);
    }

    LERROR("ModuleIssues") << oss.str() << LE;
}
