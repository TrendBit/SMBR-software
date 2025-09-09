#pragma once
#include <memory>
#include "can/CanChannel.hpp"

/**
 * @brief Monitors and handles issues received from a CAN channel.
 *
 */
class ModuleIssues {
public:
    explicit ModuleIssues(std::shared_ptr<CanChannel> channel);

private:
    void handleIssue(const CanChannel::ResponseData& response);

    std::shared_ptr<CanChannel> channel_;
};
