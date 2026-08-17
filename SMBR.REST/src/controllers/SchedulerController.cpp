#include "SchedulerController.hpp"

#include "SMBR/Exceptions.hpp"
#include "SMBR/Log.hpp"
#include "ControllerUtils.hpp"

#include <Poco/DateTimeFormatter.h>
#include <sstream>

#include <chrono>

using namespace std::chrono_literals;

SchedulerController::SchedulerController(const std::shared_ptr<oatpp::web::mime::ContentMappers>& apiContentMappers,
                                         std::shared_ptr<ISystemModule> systemModule,
                                         std::shared_ptr<IScheduler> scheduler,
                                         std::shared_ptr<IRecipes> recipes)
    : SMBRControllerBase(apiContentMappers, systemModule)
    , scheduler_(scheduler)
    , recipes_(recipes)
{}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SchedulerController::selectRecipe(const oatpp::String& name) {
    return process(__FUNCTION__, [&](){
        std::string nn = decodeRecipeName(name);
        auto sc = recipes_->getRecipeContent(nn);
        scheduler_->setScriptFromString(sc);
        auto dto = MessageDto::createShared();
        dto->message = "Recipe " + nn + " selected.";
        return createDtoResponse(Status::CODE_200, dto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SchedulerController::getRecipe() {
    return process(__FUNCTION__, [&](){
        auto s = scheduler_->getScript();
        auto scriptResponseDto = ScriptDto::createShared();
        scriptResponseDto->name = s.name;
        scriptResponseDto->content = s.content;
        return createDtoResponse(Status::CODE_200, scriptResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SchedulerController::startScheduler() {
    return process(__FUNCTION__, [&](){
        auto processId = scheduler_->start();
        auto scriptProcessIdDto = ScriptProcessIdDto::createShared();
        scriptProcessIdDto->processId = processId;
        return createDtoResponse(Status::CODE_200, scriptProcessIdDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SchedulerController::stopScheduler() {
    return process(__FUNCTION__, [&](){
        scheduler_->stop();
        auto dto = MessageDto::createShared();
        dto->message = "Script stopped successfully.";
        return createDtoResponse(Status::CODE_200, dto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SchedulerController::getSchedulerInfo() {
    return process(__FUNCTION__, [&](){
        RuntimeInfo info = scheduler_->getRuntimeInfo();
        auto infoResponseDto = ScriptRuntimeInfoDto::createShared();

        infoResponseDto->processId = info.processId;
        infoResponseDto->name = info.name;
        infoResponseDto->finalMessage = info.finishMessage;
        infoResponseDto->stack = oatpp::Vector<Int32>::createShared();
        for (auto i : info.stack){
            infoResponseDto->stack->push_back(i);
        }
        infoResponseDto->output = oatpp::Vector<String>::createShared();
        for (auto o : info.output){
            std::stringstream s;
            s << o;
            infoResponseDto->output->push_back(s.str());
        }
        infoResponseDto->started = info.started;
        infoResponseDto->stopped = info.stopped;
        if (info.started){
            infoResponseDto->startedAt = Poco::DateTimeFormatter::format(info.startTime, "%Y-%m-%d %H:%M:%S");
        }

        return createDtoResponse(Status::CODE_200, infoResponseDto);
    });
}
