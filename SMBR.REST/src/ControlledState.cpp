#include "ControlledState.hpp"

void ControlledState::set(const std::string & managerId, bool controlled) {
    std::lock_guard<std::mutex> lock(mutex_);
    flag_.active = controlled;
    flag_.managerId = controlled ? managerId : std::string();
}

ControlledState::Flag ControlledState::get() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return flag_;
}
