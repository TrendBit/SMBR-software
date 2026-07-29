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
    try {
        std::string nn = decodeRecipeName(name);
        LDEBUG("API") << "Api selectRecipe " << nn << " begin" << LE;
        auto sc = recipes_->getRecipeContent(nn);
        scheduler_->setScriptFromString(sc);
        LDEBUG("API") << "Api selectRecipe end (success)" << LE;
        auto dto = MessageDto::createShared();
        dto->message = "Recipe " + nn + " selected.";
        return createDtoResponse(Status::CODE_200, dto);
    } catch (std::exception & e){
        LWARNING("API") << "Api selectRecipe end (failure: " << e.what() << ")" << LE;
        auto dto = MessageDto::createShared();
        dto->message = "Failed to select script: " + std::string(e.what());
        return createDtoResponse(Status::CODE_404, dto);
    }
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SchedulerController::getRecipe() {
    try {
        LDEBUG("API") << "Api getRecipe begin" << LE;
        auto s = scheduler_->getScript();
        auto scriptResponseDto = ScriptDto::createShared();
        scriptResponseDto->name = s.name;
        scriptResponseDto->content = s.content;
        LDEBUG("API") << "Api getRecipe end (success)" << LE;
        return createDtoResponse(Status::CODE_200, scriptResponseDto);
    } catch (std::exception & e){
        LWARNING("API") << "Api getRecipe end (failure: " << e.what() << ")" << LE;
        auto dto = MessageDto::createShared();
        dto->message = "Failed to retrieve recipe: " + std::string(e.what());
        return createDtoResponse(Status::CODE_404, dto);
    }
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SchedulerController::startScheduler() {
    try {
        LDEBUG("API") << "Api startScheduler begin" << LE;
        auto processId = scheduler_->start();
        auto scriptProcessIdDto = ScriptProcessIdDto::createShared();
        scriptProcessIdDto->processId = processId;
        LDEBUG("API") << "Api startScheduler end (success)" << LE;
        return createDtoResponse(Status::CODE_200, scriptProcessIdDto);
    } catch (std::exception & e){
        LWARNING("API") << "Api startScheduler end (failure: " << e.what() << ")" << LE;
        auto dto = MessageDto::createShared();
        dto->message = "Failed to start scheduler: " + std::string(e.what());
        return createDtoResponse(Status::CODE_500, dto);
    }
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SchedulerController::stopScheduler() {
    try {
        LDEBUG("API") << "Api stopScheduler begin" << LE;
        scheduler_->stop();
        LDEBUG("API") << "Api stopScheduler end (success)" << LE;
        auto dto = MessageDto::createShared();
        dto->message = "Script stopped successfully.";
        return createDtoResponse(Status::CODE_200, dto);
    } catch (std::exception & e){
        LWARNING("API") << "Api stopScheduler end (failure: " << e.what() << ")" << LE;
        auto dto = MessageDto::createShared();
        dto->message = "Failed to stop script: " + std::string(e.what());
        return createDtoResponse(Status::CODE_500, dto);
    }
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SchedulerController::getSchedulerInfo() {
    try {
        LDEBUG("API") << "Api getSchedulerInfo begin" << LE;
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
        
        LDEBUG("API") << "Api getSchedulerInfo end" << LE;
        return createDtoResponse(Status::CODE_200, infoResponseDto);

    } catch (std::exception & e){
        LWARNING("API") << "Api getSchedulerInfo end (failure: " << e.what() << ")" << LE;
        auto dto = MessageDto::createShared();
        dto->message = "Failed to retrieve scheduler status: " + std::string(e.what());
        return createDtoResponse(Status::CODE_500, dto);
    }
}
