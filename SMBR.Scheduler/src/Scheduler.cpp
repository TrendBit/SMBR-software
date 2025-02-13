#include "SMBR/Scheduler.hpp"
#include "SMBR/Parser.hpp"

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

Scheduler::Scheduler(ISystemModule::Ptr systemModule) : systemModule(systemModule), t("Bg.Scheduler"), stopped(false) {
    
    bgScriptStarted = false;
    bgScriptStopped = true;

    //run method in bg thread
    t.startFunc([this](){
        run();
    });
}

Scheduler::~Scheduler(){
    stop();
    stopped = true;
    t.join();
}



void Scheduler::setScriptFromString(std::string name, const std::string & content){

    stop();

    ActiveScript::Ptr as = std::make_shared<ActiveScript>();
    as->name = name;
    as->content = content;
    as->script = Parser::parseContent(name, content);
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
    as->name = filename;
    as->content = "TODO";
    as->script = Parser::parseFile(filename);
    as->pctx = std::make_shared<ParseContext>(as->script);
    as->mainCommand = Interpreter::build(as->script.getNamedBlock("main"), as->pctx);

    {
        std::scoped_lock lock(scriptMutex);
        uploadedScript = as;
    }
}

void Scheduler::getScript(std::string & name, std::string & content) const {
    std::scoped_lock lock(scriptMutex);
    if (uploadedScript) {
        name = uploadedScript->name;
        content = uploadedScript->content;
    } else {
        throw std::runtime_error("No script uploaded");
    }
}

void Scheduler::start(){
    std::scoped_lock lock(scriptMutex);
    if (bgScriptStarted){
        throw std::runtime_error("Script already running");
    }
    if (pendingScript){
        throw std::runtime_error("Script already in queue");
    }
    if (!uploadedScript) {
        throw std::runtime_error("No script uploaded");
    } 

    bgScriptStopped = false;
    pendingScript = uploadedScript;
}
void Scheduler::stop(){
    bgScriptStopped = true;
}

Scheduler::RuntimeInfo Scheduler::runtimeInfo() const {
    std::scoped_lock lock(infoMutex);
    return info;
}

void Scheduler::run(){
    while (true){
        if (stopped) {
            break;
        }
        
        ActiveScript::Ptr as;
        {
            std::scoped_lock lock(scriptMutex);
            if (pendingScript) {
                as = pendingScript;
                pendingScript = nullptr;
            }
            runningScript = as;
        }

        if (as) {
            bgScriptStarted = true;   

            try {

                {
                    std::scoped_lock lock(infoMutex);
                    info.name = as->name;
                    info.started = true;
                    info.stopped = false;
                    info.finishMessage = "";
                    info.startTime.update();
                    info.stack.clear();
                    //std::cout << "BG [START] " << info << std::endl;
                }

                auto stack = std::make_shared<Stack>([&](const Stack::Data & data) {
                    std::scoped_lock lock(infoMutex);
                    info.stack = data;
                    //std::cout << "BG [STACK] " << info << std::endl;
                });

                auto rctx = std::make_shared<RunContext>(stack, systemModule);
                
                rctx->stopCb = [&](){
                    return !bgScriptStarted || bgScriptStopped;
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

                as->mainCommand->run(rctx);

                {
                    std::scoped_lock lock(infoMutex);
                    info.stopped = true;
                    info.finishMessage = "Successfully finished";
                    //std::cout << "BG [SUCCESS] " << info << std::endl;
                }

            } catch (std::exception & e) {
                std::scoped_lock lock(infoMutex);
                info.stopped = true;
                std::string emsg = e.what();
                info.finishMessage = "Failed: " + emsg;
                //std::cout << "BG [ERROR] " << info << std::endl;
            }

            bgScriptStarted = false;   
        }
        //sleep for 1s
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

std::ostream & operator << (std::ostream & os, const Scheduler::RuntimeInfo & info){
    os << "Scheduler: " << info.name << " started: " << info.started << " stopped: " << info.stopped << " finish: " << info.finishMessage;
    return os;
}
