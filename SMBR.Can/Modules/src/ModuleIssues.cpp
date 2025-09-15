#include "ModuleIssues.hpp"
#include "SMBR/Log.hpp"
#include "can/CanMessage.hpp"
#include <sstream>
#include <iomanip>
#include <ctime>

using namespace App_messages::Module_issue;

namespace {

std::string toString(IssueType type) {
    switch(type) {
        case IssueType::HighLoad:                return "HighLoad";
        case IssueType::CoreOverTemp:            return "CoreOverTemp";
        case IssueType::BoardOverTemp:           return "BoardOverTemp";
        case IssueType::LowEEPROMMemory:         return "LowEEPROMMemory";
        case IssueType::Invalid5VSupply:         return "Invalid5VSupply";
        case IssueType::InvalidVinSupply:        return "InvalidVinSupply";
        case IssueType::InvalidPoESupply:        return "InvalidPoESupply";
        case IssueType::OverCurrentDraw:         return "OverCurrentDraw";
        case IssueType::OverPowerDraw:           return "OverPowerDraw";
        case IssueType::LEDPanelOverTemp:        return "LEDPanelOverTemp";
        case IssueType::HeaterOverTemp:          return "HeaterOverTemp";
        case IssueType::PumpInvalidSpeed:        return "PumpInvalidSpeed";
        case IssueType::PumpInvalidFlowrate:     return "PumpInvalidFlowrate";
        case IssueType::AeratorOverSpeed:        return "AeratorOverSpeed";
        case IssueType::AeratorInvalidFlowrate:  return "AeratorInvalidFlowrate";
        case IssueType::MixerOverSpeed:          return "MixerOverSpeed";
        case IssueType::MixerOverRPM:            return "MixerOverRPM";
        case IssueType::BottleOverTemp:          return "BottleOverTemp";
        case IssueType::BottleTopOverMeasTemp:   return "BottleTopOverMeasTemp";
        case IssueType::BottleBottomOverMeasTemp:return "BottleBottomOverMeasTemp";
        case IssueType::BottleTopOverSensorTemp: return "BottleTopOverSensorTemp";
        case IssueType::BottleBottomOverSensorTemp:return "BottleBottomOverSensorTemp";
        case IssueType::FluorometerDetectorOverTemp:return "FluorometerDetectorOverTemp";
        case IssueType::FluorometerEmitorOverTemp:return "FluorometerEmitorOverTemp";
        case IssueType::SpectrophotometerEmitorOverTemp:return "SpectrophotometerEmitorOverTemp";
        default: return "Unknown";
    }
}

std::string toString(Severity sev) {
    switch(sev) {
        case Severity::Info:     return "Info";
        case Severity::Warning:  return "Warning";
        case Severity::Error:    return "Error";
        case Severity::Critical: return "Critical";
        default: return "Unknown";
    }
}

std::string toIso8601(const std::chrono::steady_clock::time_point& tp) {
    auto now_sys = std::chrono::system_clock::now();
    auto now_steady = std::chrono::steady_clock::now();
    auto sys_tp = now_sys + (tp - now_steady);

    std::time_t timeT = std::chrono::system_clock::to_time_t(sys_tp);

    std::ostringstream oss;
    oss << std::put_time(std::localtime(&timeT), "%Y-%m-%dT%H:%M:%S");
    return oss.str();
}

} 

ModuleIssues::ModuleIssues(std::shared_ptr<CanChannel> channel)
    : channel_(channel)
{
    channel_->setReceiveHandler([this](const CanChannel::ResponseData& response){
        handleIssue(response);
    });
}

void ModuleIssues::handleIssue(const CanChannel::ResponseData& response) {
    CanID canid(response.id);

    Module_issue msg;
    can_data_vector_t data = response.data;  
    if (!msg.Interpret_data(data)) {
        return;
    }

    Modules moduleType;
    switch (canid.moduleAddress()) {
        case CanID::Core_module:    moduleType = Modules::Core; break;
        case CanID::Control_module: moduleType = Modules::Control; break;
        case CanID::Sensor_module:  moduleType = Modules::Sensor; break;
        default:                    moduleType = Modules::Unknown; break;
    }

    ModuleID module(
        moduleType, 
        "",                   
        static_cast<Instance>(canid.instance())
    );

    IssueKey key = { module, msg.error_type };

    issues_[key] = IssueRecord{
        msg.error_type,
        msg.severity,
        msg.value,
        module,
        std::chrono::steady_clock::now()
    };
}

std::vector<ModuleIssues::IssueRecord> ModuleIssues::getActiveIssues() {
    std::vector<IssueRecord> active;
    auto now = std::chrono::steady_clock::now();

    for (auto it = issues_.begin(); it != issues_.end(); ) {
        auto age = std::chrono::duration_cast<std::chrono::seconds>(
            now - it->second.lastSeen);

        if (age.count() > 180) {
            it = issues_.erase(it); 
        } else {
            active.push_back(it->second);
            ++it;
        }
    }
    return active;
}

std::vector<IModuleIssues::ModuleIssueData> ModuleIssues::getActiveIssuesData() {
    std::vector<IModuleIssues::ModuleIssueData> result;
    auto active = getActiveIssues();

    for (const auto& issue : active) {
        IModuleIssues::ModuleIssueData data;
        data.error_type = issue.error_type;
        data.name = toString(static_cast<IssueType>(issue.error_type));
        data.severity = toString(static_cast<Severity>(issue.severity));
        data.timestamp = toIso8601(issue.lastSeen);
        data.value = issue.value;
        data.module = issue.module;   
        result.push_back(std::move(data));
    }
    return result;
}

void ModuleIssues::printActiveIssues() {
    auto active = getActiveIssues();

    for (const auto& issue : active) {
        std::ostringstream oss;
        oss << "Module type=" << static_cast<int>(issue.module.type)
            << " instance=" << static_cast<int>(issue.module.instance);

        if (!issue.module.uidHex.empty()) {
            oss << " uid=" << issue.module.uidHex;
        }

        oss << " issue=" << toString(static_cast<IssueType>(issue.error_type))
            << " severity=" << toString(static_cast<Severity>(issue.severity))
            << " value=" << issue.value
            << " timestamp=" << toIso8601(issue.lastSeen);

        LERROR("ModuleIssues") << oss.str() << LE;
    }
}
