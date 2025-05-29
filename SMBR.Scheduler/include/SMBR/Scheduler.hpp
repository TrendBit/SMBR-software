#pragma once

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <set>
#include <thread>
#include <atomic>
#include <deque>

#include <SMBR/IScheduler.hpp>
#include <SMBR/Script.hpp>
#include <SMBR/ISystemModule.hpp>
#include <SMBR/Interpreter.hpp>
#include <Poco/Timestamp.h>
#include <Poco/Thread.h>

class Scheduler : public IScheduler {
    
    public:    
        typedef std::shared_ptr <Scheduler> Ptr;
        
        Scheduler(ISystemModule::Ptr systemModule);
        ~Scheduler();

        void setScriptFromString(const ScriptInfo & s) override;
        void setScriptFromFile(const std::string & filename) override;

        unsigned long long start() override; 
        void stop() override;

        ScriptInfo getScript() const override;
        RuntimeInfo getRuntimeInfo() const override;
        
    private:
        void run();
        
    private:

        ISystemModule::Ptr systemModule;

        struct ActiveScript {
            typedef std::shared_ptr <ActiveScript> Ptr;
            ScriptInfo info;
            Script script;
            ParseContext::Ptr pctx;
            ICommand::Ptr mainCommand;
        };

        struct StartedScript {
            ActiveScript::Ptr script;
            unsigned long long processId = 0;
        };

        void assignNewInfo(ActiveScript::Ptr as);

        Poco::Thread t;
        std::atomic_bool stopped;
        
        mutable std::mutex scriptMutex;
        ActiveScript::Ptr uploadedScript;
        StartedScript pendingScript;
       

        mutable std::mutex infoMutex;
        RuntimeInfo info;

        std::atomic_bool bgScriptStarted;
        std::atomic_bool bgScriptStopped;

        std::atomic_ullong processId = 1;
};

