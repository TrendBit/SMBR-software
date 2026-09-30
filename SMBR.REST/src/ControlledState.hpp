#pragma once

#include <mutex>
#include <string>

/**
 * @class ControlledState
 * @brief Whether a manager currently actively controls this reactor, and which one.
 */
class ControlledState {
public:
    struct Flag {
        bool active = false;
        std::string managerId;
    };

    void set(const std::string & managerId, bool controlled);
    Flag get() const;

private:
    mutable std::mutex mutex_;
    Flag flag_;
};
