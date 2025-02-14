#include "SMBR/APIScheduler.hpp"
#include "dto/MyScriptDto.hpp"
#include "dto/MyScriptRuntimeInfoDto.hpp"

#include "APIClientImpl.hpp"
#include <fstream>



APIScheduler::APIScheduler(APIClientParams params)
    : impl(std::make_shared<APIClientImpl>(params))
{}

APIScheduler::~APIScheduler() {}

unsigned long long APIScheduler::start() {
    auto response = impl->client()->startScheduler();
    if (response->getStatusCode() != 200) {
        throw std::runtime_error("start scheduler error TODO");
    }
    auto processIdDto = response->readBodyToDto<oatpp::Object<MyScriptProcessIdDto>>(impl->objectMapper());
    return processIdDto->processId;
}

void APIScheduler::stop() {
    auto response = impl->client()->stopScheduler();
    if (response->getStatusCode() != 200) {
        throw std::runtime_error("stop scheduler error TODO");
    }
}

void APIScheduler::setScriptFromString(const ScriptInfo & info) {
    auto scriptDto = MyScriptDto::createShared();
    scriptDto->name = info.name;
    scriptDto->content = info.content;
    auto response = impl->client()->uploadScript(scriptDto);
    if (response->getStatusCode() != 200) {
        throw std::runtime_error("set script error TODO");
    }

}

void APIScheduler::setScriptFromFile(const std::string & filename) {
    //read complete file into string
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Cannot open file " + filename);
    }
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    setScriptFromString({filename, content});
}

ScriptInfo APIScheduler::getScript() const {

    auto response = impl->client()->getScript();

    if (response->getStatusCode() == 200) {
        auto scriptDto = response->readBodyToDto<oatpp::Object<MyScriptDto>>(impl->objectMapper());
        ScriptInfo si;
        si.name = scriptDto->name; 
        si.content = scriptDto->content;
        return si;
    } else {
        throw std::runtime_error("get scrfipt error TODO");
    }
}

RuntimeInfo APIScheduler::getRuntimeInfo() const {
    
    auto response = impl->client()->getSchedulerInfo();
    
    if (response->getStatusCode() == 200) {
        //auto runtimeInfoDto = response->readBodyToDto<MyScriptRuntimeInfoDto>(impl->objectMapper());
        auto runtimeInfoDto = response->readBodyToDto<oatpp::Object<MyScriptRuntimeInfoDto>>(impl->objectMapper());

        RuntimeInfo ri;
        ri.started = runtimeInfoDto->started;
        ri.stopped = runtimeInfoDto->stopped;
        ri.name = runtimeInfoDto->name;
        ri.finishMessage = runtimeInfoDto->finalMessage;
        ri.processId = runtimeInfoDto->processId;
        //TODO stack + output
        
        return ri;
    } else {
        throw std::runtime_error("get runtime info error TODO");
    }
    
}


