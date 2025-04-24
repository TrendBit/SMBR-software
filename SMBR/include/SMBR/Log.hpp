#pragma once

#include <Poco/Logger.h>
#include <Poco/LogStream.h>
#include <Poco/NullStream.h>
#include <set>
#include <string>

namespace SMBR {
    std::string logBasename(const std::string& fullname);
}

//#define LE " (in " << SMBR::logBasename(__FILE__) << ":" << __func__ << ":" << __LINE__ << ")" << std::endl; }}
#define LE std::endl; }}

#define LFATAL(name) { Poco::Logger & pl = Poco::Logger::get(name); Poco::LogStream str(pl); if (pl.fatal()) { str.fatal()
#define LCRITICAL(name) { Poco::Logger & pl = Poco::Logger::get(name); Poco::LogStream str(pl); if (pl.critical()) { str.critical()
#define LERROR(name) { Poco::Logger & pl = Poco::Logger::get(name); Poco::LogStream str(pl); if (pl.error()) { str.error()
#define LWARNING(name)  { Poco::Logger & pl = Poco::Logger::get(name); Poco::LogStream str(pl); if (pl.warning()) { str.warning()
#define LNOTICE(name)  { Poco::Logger & pl = Poco::Logger::get(name); Poco::LogStream str(pl); if (pl.notice()) { str.notice()
#define LINFO(name) { Poco::Logger & pl = Poco::Logger::get(name); Poco::LogStream str(pl); if (pl.information()) { str.information()
#define LDEBUG(name) { Poco::Logger & pl = Poco::Logger::get(name); Poco::LogStream str(pl); if (pl.debug()) { str.debug()
#define LTRACE(name) { Poco::Logger & pl = Poco::Logger::get(name); Poco::LogStream str(pl); if (pl.trace()) { str.trace()
#define LLEVEL(name, prio) { Poco::Logger & pl = Poco::Logger::get(name); Poco::LogStream str(pl); if (pl.is(prio)) { str.priority(prio)

namespace SMBR {
    
    void threadDebug();

   
    void dumpBacktrace(std::string name, std::string logname = "SMBR", int level = 4);

    void initConsoleLogs(int level = Poco::Message::PRIO_TRACE);
}

