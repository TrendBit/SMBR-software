#pragma once
#include <memory>
#include <chrono>
#include <map>
#include <vector>
#include <string>
#include "can/CanChannel.hpp"
#include "codes/messages/module_issues/module_issue.hpp"
#include "SMBR/IModuleIssues.hpp"
#include "SMBR/SMBR.hpp" 

/**
 * @brief Monitors and handles issues received from a CAN channel.
 *
 */
class ModuleIssues : public IModuleIssues {
public:
    explicit ModuleIssues(std::shared_ptr<CanChannel> channel);
    
    std::vector<ModuleIssueData> getActiveIssuesData() override;

private:
    struct IssueRecord {
        uint16_t error_type;
        int16_t index;
        float value;
        ModuleID module;   
        std::chrono::steady_clock::time_point lastSeen;
    };

    struct IssueKey {
        ModuleID module;
        uint16_t error_type;

        bool operator<(const IssueKey& other) const {
            return (module < other.module) || 
                   (!(other.module < module) && error_type < other.error_type);
        }
    };

    std::vector<IssueRecord> getActiveIssues();
    void handleIssue(const CanChannel::ResponseData& response);
    void printActiveIssues();
    
    std::shared_ptr<CanChannel> channel_;
    std::map<IssueKey, IssueRecord> issues_;
};
