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

enum Instance {
    Undefined   = 0x00,
    Exclusive   = 0x01,
    All         = 0x02,
    Reserved    = 0x03,
    Instance_1  = 0x04,
    Instance_2  = 0x05,
    Instance_3  = 0x06,
    Instance_4  = 0x07,
    Instance_5  = 0x08,
    Instance_6  = 0x09,
    Instance_7  = 0x0a,
    Instance_8  = 0x0b,
    Instance_9  = 0x0c,
    Instance_10 = 0x0d,
    Instance_11 = 0x0e,
    Instance_12 = 0x0f,
};

struct ModuleID {
    Modules type;
    std::string uidHex;
    Instance instance;

    ModuleID() : type(Core), instance(Instance::Undefined) {}
    ModuleID(Modules m) : type(m), instance(Instance::Undefined) {}
    ModuleID(Modules m, std::string uidHex, Instance inst = Instance::Undefined) 
        : type(m), uidHex(uidHex), instance(inst) {}

    bool operator==(const ModuleID& other) const {
        return type == other.type && uidHex == other.uidHex && instance == other.instance;
    }
    bool operator!=(const ModuleID& other) const {
        return !(*this == other);
    }
    bool operator<(const ModuleID& other) const {
        return type < other.type || (type == other.type && uidHex < other.uidHex) || 
               (type == other.type && uidHex == other.uidHex && instance < other.instance);
    }
};

std::ostream& operator<<(std::ostream& os, const Modules& module);
std::ostream& operator<<(std::ostream& os, const Instance& instance);
std::ostream& operator<<(std::ostream& os, const ModuleID& moduleid);