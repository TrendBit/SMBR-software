#include "SMBR/SMBR.hpp"

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

std::ostream& operator<<(std::ostream& os, const Instance& instance){
    switch (instance) {
        case Instance::Undefined:
            os << "Undefined";
            break;
        case Instance::Exclusive:
            os << "Exclusive";
            break;
        case Instance::All:
            os << "All";
            break;
        case Instance::Reserved:
            os << "Reserved";
            break;
        case Instance::Instance_1:
            os << "Instance_1";
            break;
        case Instance::Instance_2:
            os << "Instance_2";
            break;
        case Instance::Instance_3:
            os << "Instance_3";
            break;
        case Instance::Instance_4:
            os << "Instance_4";
            break;
        case Instance::Instance_5:
            os << "Instance_5";
            break;
        case Instance::Instance_6:
            os << "Instance_6";
            break;
        case Instance::Instance_7:
            os << "Instance_7";
            break;
        case Instance::Instance_8:
            os << "Instance_8";
            break;
        case Instance::Instance_9:
            os << "Instance_9";
            break;
        case Instance::Instance_10:
            os << "Instance_10";
            break;
        case Instance::Instance_11:
            os << "Instance_11";
            break;
        case Instance::Instance_12:
            os << "Instance_12";
            break;
        default:
            os << "Unknown";
            break;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const ModuleID& moduleid){
    os << moduleid.type << ":" << moduleid.uidHex << ":" << moduleid.instance;
    return os;
}