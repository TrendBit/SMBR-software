#pragma once

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <set>
#include <thread>
#include <atomic>
#include <deque>

#include <SMBR/Script.hpp>
#include <SMBR/ISystemModule.hpp>
#include <SMBR/Interpreter.hpp>
#include <Poco/Timestamp.h>
#include <Poco/Thread.h>

class Scheduler {
    
    public:    
        typedef std::shared_ptr <Scheduler> Ptr;
        
        Scheduler(ISystemModule::Ptr systemModule);
        ~Scheduler();

        void setScriptFromString(std::string name, const std::string & content);
        void setScriptFromFile(const std::string & filename);

        void getScript(std::string & name, std::string & content) const;
        
        void start();
        void stop();

        struct OutputLine {
            Poco::Timestamp time; 
            std::string message;
        };

        struct RuntimeInfo {
            std::string name;
            bool started = false;
            bool stopped = false;
            std::deque <OutputLine> output;
            std::string finishMessage;
            Poco::Timestamp startTime;
            Stack::Data stack;    
        };
        RuntimeInfo runtimeInfo() const;

    private:
        void run();
        
    private:

        ISystemModule::Ptr systemModule;

        struct ActiveScript {
            typedef std::shared_ptr <ActiveScript> Ptr;
            std::string name;
            std::string content;
            Script script;
            ParseContext::Ptr pctx;
            ICommand::Ptr mainCommand;
        };

        Poco::Thread t;
        std::atomic_bool stopped;
        
        mutable std::mutex scriptMutex;
        ActiveScript::Ptr pendingScript;
        ActiveScript::Ptr uploadedScript;
        ActiveScript::Ptr runningScript;
       

        mutable std::mutex infoMutex;
        RuntimeInfo info;

        std::atomic_bool bgScriptStarted;
        std::atomic_bool bgScriptStopped;
};

std::ostream & operator << (std::ostream & os, const Scheduler::RuntimeInfo & info);

