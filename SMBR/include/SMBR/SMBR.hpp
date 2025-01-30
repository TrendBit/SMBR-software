#pragma once

#include <memory>
#include <string>
#include <ostream>  

enum Modules {
    Core = 1,
    Control = 2,
    Sensor = 3
};

struct ModuleID {
    Modules type;
    int uid = 0;

    ModuleID() : type(Core){}
    ModuleID(Modules m, int uid = 0) : type(m), uid(uid){}

    bool operator==(const ModuleID& other) const {
        return type == other.type && uid == other.uid;
    }
    bool operator!=(const ModuleID& other) const {
        return !(*this == other);
    }
    bool operator<(const ModuleID& other) const {
        return type < other.type || (type == other.type && uid < other.uid);
    }
};


std::ostream& operator<<(std::ostream& os, const Modules& module);
std::ostream& operator<<(std::ostream& os, const ModuleID& moduleid);

