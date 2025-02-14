#include "SMBR/IScheduler.hpp"
#include "Poco/DateTimeFormatter.h"

std::ostream& operator<<(std::ostream& os, const OutputLine& o){
    os << Poco::DateTimeFormatter::format(o.time, "%Y-%m-%d %H:%M:%S");
    os << " " << o.message;
    return os;
}
std::ostream& operator<<(std::ostream& os, const RuntimeInfo& info){
    os << "#" << info.processId << " " << info.name << " started: " << info.started << " stopped: " << info.stopped << " finish: " << info.finishMessage;
    return os;
}
std::ostream& operator<<(std::ostream& os, const ScriptInfo& info){
    os << "name: " << info.name << " content: " << info.content;
    return os;
}
