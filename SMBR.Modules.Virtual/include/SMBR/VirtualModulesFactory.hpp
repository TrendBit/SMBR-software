#pragma once

#include <SMBR/ISystemModule.hpp>

class VirtualModulesFactory {
public:
    static std::shared_ptr<ISystemModule> create();
};