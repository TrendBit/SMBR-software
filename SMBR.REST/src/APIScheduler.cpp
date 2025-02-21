#include "SMBR/APIScheduler.hpp"
#include "dto/ScriptDto.hpp"
#include "dto/ScriptRuntimeInfoDto.hpp"

#include "APIClientImpl.hpp"
#include <fstream>



APIScheduler::APIScheduler(APIClientParams params)
    : impl(std::make_shared<APIClientImpl>(params))
{}

APIScheduler::~APIScheduler() {}

static void checkResponse(int code, std::string description, const std::string & message) {
    throw std::runtime_error("Error: " + std::to_string(code) + " " + description + " - " + message);
}
static void checkResponse(std::shared_ptr<oatpp::web::protocol::http::incoming::Response> response){
    if (response->getStatusCode() != 200) {
        checkResponse(response->getStatusCode(), response->getStatusDescription(), response->readBodyToString()->c_str());
    }
    
}

unsigned long long APIScheduler::start() {
    auto response = impl->client()->startScheduler();
    checkResponse(response);

    auto processIdDto = response->readBodyToDto<oatpp::Object<ScriptProcessIdDto>>(impl->objectMapper());
    return processIdDto->processId;
}

void APIScheduler::stop() {
    auto response = impl->client()->stopScheduler();
    checkResponse(response);
}

void APIScheduler::setScriptFromString(const ScriptInfo & info) {
    auto scriptDto = ScriptDto::createShared();
    scriptDto->name = info.name;
    scriptDto->content = info.content;
    auto response = impl->client()->uploadScript(scriptDto);
    checkResponse(response);
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

    checkResponse(response);

    auto scriptDto = response->readBodyToDto<oatpp::Object<ScriptDto>>(impl->objectMapper());
    ScriptInfo si;
    si.name = scriptDto->name; 
    si.content = scriptDto->content;
    return si;

}

RuntimeInfo APIScheduler::getRuntimeInfo() const {
    
    auto response = impl->client()->getSchedulerInfo();

    checkResponse(response);

    //auto runtimeInfoDto = response->readBodyToDto<ScriptRuntimeInfoDto>(impl->objectMapper());
    auto runtimeInfoDto = response->readBodyToDto<oatpp::Object<ScriptRuntimeInfoDto>>(impl->objectMapper());

    RuntimeInfo ri;
    ri.started = runtimeInfoDto->started;
    ri.stopped = runtimeInfoDto->stopped;
    ri.name = runtimeInfoDto->name;
    ri.finishMessage = runtimeInfoDto->finalMessage;
    ri.processId = runtimeInfoDto->processId;

    for (auto it = runtimeInfoDto->stack->begin(); it != runtimeInfoDto->stack->end(); it++) {
        ri.stack.push_back(*it);
    }

    for (auto it = runtimeInfoDto->output->begin(); it != runtimeInfoDto->output->end(); it++) {
        OutputLine o;
        o.message = *it;
        ri.output.push_back(o);
        //ri.output.push_back(*it);
    }

    /*runtimeInfoDto->output->forEach([&ri](const ScriptOutputLineDto & data){
        ri.output.push_back(data.message);
    });*/
    return ri;

    
}


