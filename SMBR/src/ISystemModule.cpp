#include "SMBR/ISystemModule.hpp"

std::ostream& operator<<(std::ostream& os, const Modules& module){
    switch (module) {
        case Modules::Core:
            os << "Core";
            break;
        case Modules::Control:
            os << "Control";
            break;
        case Modules::Sensor:
            os << "Sensor";
            break;
        default:
            os << "Unknown";
            break;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const ISystemModule::AvailableModules& modules){
    os << "[";
    for (const auto& module : modules){
        os << module << " ";
    }
    os << "]";
    return os;
}