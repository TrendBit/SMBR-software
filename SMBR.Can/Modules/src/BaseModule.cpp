#include "BaseModule.hpp"
#include <iomanip>
#include <sstream>
#include <Poco/Mutex.h>
#include <Poco/NumberFormatter.h>

static Codes::Module module2raw(Modules module){
    switch (module) {
        case Modules::Core:
            return Codes::Module::Core_module;
        case Modules::Control:
            return Codes::Module::Control_module;
        case Modules::Sensor:
            return Codes::Module::Sensor_module;
        default:
            return Codes::Module::Undefined;
    }
}

CanID BaseModule::createId(Codes::Message_type messageType, Modules module, Codes::Instance instance, bool emergencyFlag){
    return createId(messageType, module2raw(module), instance, emergencyFlag);
}

CanID BaseModule::createId(Codes::Message_type messageType, Codes::Module module, Codes::Instance instance, bool emergencyFlag){
    uint32_t id = 0;
    id |= (emergencyFlag ? 1 : 0) << 28;
    id |= static_cast<uint32_t>(messageType) << 16;
    id |= static_cast<uint32_t>(module) << 4;
    id |= static_cast<uint32_t>(instance);
    return id;
}

BaseModule::BaseModule(ModuleID i, ICanChannel::Ptr channel) : id_(i), channel(channel) {}

Modules BaseModule::module() const {
    return id_.type;
}

std::string BaseModule::uidHex() const {
    return id_.uidHex;
}

ModuleID BaseModule::id() const {
    return id_;
}

CanID BaseModule::createRequestId(Codes::Message_type messageType, Codes::Instance instance, bool emergencyFlag){
    return BaseModule::createId(messageType, module(), instance, emergencyFlag);
}

std::string BaseModule::requestLogStr(CanID reqId){
    static Poco::Int64 seqShared = 0;
    Poco::Int64 seq = 0;
    static Poco::Mutex m;
    {
        Poco::Mutex::ScopedLock l(m);
        seq = seqShared++;
    }
    
    std::stringstream s;
    s << "[" << Poco::NumberFormatter::format0(seq, 10) << " " << reqId << "]";
    return s.str();
}
        
 