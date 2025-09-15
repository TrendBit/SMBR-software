#pragma once
#include <memory>
#include "SMBR/SMBR.hpp"

/**
 * @brief Interface for accessing active issues reported by a module.
 *
 */
class IModuleIssues {
public:
    using Ptr = std::shared_ptr<IModuleIssues>;
    virtual ~IModuleIssues() = default;

    /**
     * @brief Structure representing a single issue reported by a module.
     */
    struct ModuleIssueData {
        uint8_t error_type;
        std::string name;
        std::string severity;
        std::string timestamp;
        float value;
        ModuleID module;
    };

    /**
     * @brief Returns a list of currently active issues.
     */
    virtual std::vector<ModuleIssueData> getActiveIssuesData() = 0;
};
