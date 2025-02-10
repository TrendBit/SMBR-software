#pragma once

#include <memory>
#include <string>
#include <ostream>  

enum Modules {
    Unknown = 0,
    Core = 1,
    Control = 2,
    Sensor = 3
};

struct ModuleID {
    Modules type;
    std::string uidHex;

    ModuleID() : type(Core){}
    ModuleID(Modules m) : type(m){}
    ModuleID(Modules m, std::string uidHex) : type(m), uidHex(uidHex){}

    bool operator==(const ModuleID& other) const {
        return type == other.type && uidHex == other.uidHex;
    }
    bool operator!=(const ModuleID& other) const {
        return !(*this == other);
    }
    bool operator<(const ModuleID& other) const {
        return type < other.type || (type == other.type && uidHex < other.uidHex);
    }
};


std::ostream& operator<<(std::ostream& os, const Modules& module);
std::ostream& operator<<(std::ostream& os, const ModuleID& moduleid);

