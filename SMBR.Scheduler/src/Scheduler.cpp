#include "SMBR/Scheduler.hpp"
#include "SMBR/Parser.hpp"
#include "SMBR/Log.hpp"

#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <stack>
#include <chrono>
#include <fstream>
#include <memory>
#include <list>
#include <iostream>

static const int OutputBufferSize = 100;

Scheduler::Scheduler(ISystemModule::Ptr systemModule) : systemModule(systemModule), t("scheduler.bg"), stopped(false) {
    
    bgScriptStarted = false;
    bgScriptStopped = true;

    //run method in bg thread
    t.startFunc([this](){
        SMBR::threadDebug();
        run();
    });
}

Scheduler::~Scheduler(){
    stop();
    stopped = true;
    t.join();
}



void Scheduler::setScriptFromString(const ScriptInfo & s){

    stop();

    ActiveScript::Ptr as = std::make_shared<ActiveScript>();
    as->info = s;
    as->script = Parser::parseContent(s.name, s.content);
    as->pctx = std::make_shared<ParseContext>(as->script);
    as->mainCommand = Interpreter::build(as->script.getNamedBlock("main"), as->pctx);

    {
        std::scoped_lock lock(scriptMutex);
        uploadedScript = as;
    }
}

void Scheduler::setScriptFromFile(const std::string & filename){

    stop();    

    ActiveScript::Ptr as = std::make_shared<ActiveScript>();
    as->info.name = filename;
    as->script = Parser::parseFile(filename);
    as->info.content = as->script.serialize();
    as->pctx = std::make_shared<ParseContext>(as->script);
    as->mainCommand = Interpreter::build(as->script.getNamedBlock("main"), as->pctx);

    {
        std::scoped_lock lock(scriptMutex);
        uploadedScript = as;
    }
}

ScriptInfo Scheduler::getScript() const {
    std::scoped_lock lock(scriptMutex);
    if (uploadedScript) {
        return uploadedScript->info;
    } else {
        throw std::runtime_error("No script uploaded");
    }
}

unsigned long long Scheduler::start(){
    std::scoped_lock lock(scriptMutex);
    if (bgScriptStarted){
        throw std::runtime_error("Script already running");
    }
    if (pendingScript.script){
        throw std::runtime_error("Script already in queue");
    }
    if (!uploadedScript) {
        throw std::runtime_error("No script uploaded");
    } 

    std::cout << "start called" << std::endl;
    bgScriptStopped = false;

    pendingScript.script = uploadedScript;
    pendingScript.processId = ++processId;

    return pendingScript.processId;
}
void Scheduler::stop(){
    std::cout << "stopped called" << std::endl;
    
    bgScriptStopped = true;
}

RuntimeInfo Scheduler::getRuntimeInfo() const {
    std::scoped_lock lock(infoMutex);
    return info;
}


static void stopScript(ISystemModule::Ptr systemModule){
    //stop all modules
    auto call = [](std::future<bool> f){
        try {
            f.wait();
            f.get();
        } catch (...){
        }
    };

    call(systemModule->controlModule()->setIntensities(0, 0, 0, 0));
    call(systemModule->controlModule()->turnOffHeater());
    call(systemModule->controlModule()->stopCuvettePump());
    call(systemModule->controlModule()->stopAerator());
    call(systemModule->controlModule()->stopMixer());
}

void Scheduler::run(){
    while (true){
        if (stopped) {
            break;
        }
        
        StartedScript as;
        {
            std::scoped_lock lock(scriptMutex);
            if (pendingScript.script) {
                as = pendingScript;
                pendingScript = {};
            }
        }

        if (as.script) {
            bgScriptStarted = true;   

            try {

                {
                    std::scoped_lock lock(infoMutex);
                    info.name = as.script->info.name;
                    info.processId = as.processId;
                    info.started = true;
                    info.stopped = false;
                    info.finishMessage = "";
                    info.startTime.update();
                    info.stack.clear();
                    std::cout << "BG [START] " << info << std::endl;
                }

                auto stack = std::make_shared<Stack>([&](const Stack::Data & data) {
                    std::scoped_lock lock(infoMutex);
                    info.stack = data;
                    //std::cout << "BG [STACK] " << info << std::endl;
                });

                auto rctx = std::make_shared<RunContext>(stack, systemModule);
                
                rctx->stopCb = [&](){
                    if (!bgScriptStarted || bgScriptStopped){
                        std::cout << "BG [CONDITION] " << "bgScriptStarted " << bgScriptStarted << " bgScriptStopped " << bgScriptStopped << std::endl;
                        return true;
                    }
                    return false;
                };      

                rctx->printCb = [&](const std::string & msg){
                    std::scoped_lock lock(infoMutex);
                    OutputLine ol;
                    ol.message = msg;

                    info.output.push_back(ol);
                    if (info.output.size() > OutputBufferSize) {
                        info.output.pop_front();
                    }

                    //std::cout << "BG [PRINT] " << info << std::endl;
                };

                as.script->mainCommand->run(rctx);

                {
                    std::scoped_lock lock(infoMutex);
                    info.stopped = true;
                    info.finishMessage = "Successfully finished";
                    std::cout << "BG [SUCCESS] " << info << std::endl;
                }

            } catch (std::exception & e) {
                std::scoped_lock lock(infoMutex);
                info.stopped = true;
                std::string emsg = e.what();
                info.finishMessage = "Failed: " + emsg;
                std::cout << "BG [ERROR] " << info << std::endl;
            }

            try {
                stopScript(systemModule);
            } catch (...){

            }

            bgScriptStarted = false;   
        }
        //sleep for 1s
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

