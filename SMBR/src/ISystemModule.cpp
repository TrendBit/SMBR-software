#include "SMBR/ISystemModule.hpp"


std::ostream& operator<<(std::ostream& os, const ISystemModule::AvailableModules& modules){
    os << "[";
    for (const auto& module : modules){
        os << module << " ";
    }
    os << "]";
    return os;
}