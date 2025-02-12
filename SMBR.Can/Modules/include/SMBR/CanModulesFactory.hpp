#pragma once

#include <SMBR/ISystemModule.hpp>

class CanModulesFactory {
public:
    static std::shared_ptr<ISystemModule> create();
};