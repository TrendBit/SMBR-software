#pragma once

#include <memory>
#include <string>
#include <deque>
#include <ostream>
#include <Poco/Timestamp.h>
#include <vector>

struct ScriptInfo {
    std::string name;
    std::string content;
};

struct OutputLine {
    Poco::Timestamp time; 
    std::string message;
};

struct RuntimeInfo {
    unsigned long long processId = 0;
    std::string name;
    bool started = false;
    bool stopped = false;
    std::deque <OutputLine> output;
    std::deque <OutputLine> errors;
    std::string finishMessage;
    Poco::Timestamp startTime;
    std::vector <int> stack;    
};


class IScheduler {
    public:
        typedef std::shared_ptr <IScheduler> Ptr;

        virtual ~IScheduler() = default;

        /**
         * Start the script, returns process id
         */
        virtual unsigned long long start() = 0;
        virtual void stop() = 0;

        virtual void setScriptFromString(const ScriptInfo & ) = 0;
        virtual void setScriptFromFile(const std::string & filename) = 0;

        virtual ScriptInfo getScript() const = 0;
        virtual RuntimeInfo getRuntimeInfo() const = 0;
};

std::ostream& operator<<(std::ostream& os, const OutputLine& line);
std::ostream& operator<<(std::ostream& os, const RuntimeInfo& info);
std::ostream& operator<<(std::ostream& os, const ScriptInfo& info);
