#pragma once

#include "BaseController.hpp"

#include "dto/MessageDto.hpp"
#include "dto/ScriptDto.hpp"
#include "dto/ScriptProcessIdDto.hpp"
#include "dto/ScriptRuntimeInfoDto.hpp"

#include "SMBR/IScheduler.hpp"
#include "SMBR/IRecipes.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

#undef OATPP_MACRO_API_CONTROLLER_ADD_CORS_BODY_DEFAULT_METHODS
#define OATPP_MACRO_API_CONTROLLER_ADD_CORS_BODY_DEFAULT_METHODS "GET, POST, OPTIONS, PUT, PATCH, DELETE"

/**
 * @class SchedulerController
 * @brief Endpoints controlling the recipe scheduler.
 */
class SchedulerController : public SMBRControllerBase {
public:
    SchedulerController(const std::shared_ptr<oatpp::web::mime::ContentMappers>& apiContentMappers,
                        std::shared_ptr<ISystemModule> systemModule,
                        std::shared_ptr<IScheduler> scheduler,
                        std::shared_ptr<IRecipes> recipes);

    /**
     * @brief Retrieves the currently assigned recipe.
     */
    ENDPOINT_INFO(getRecipe) {
        info->summary = "Get scheduled recipe";
        info->addTag("Scheduler");
        info->description = "Retrieves the active (selected) recipe.";
        auto exampleRecipe = ScriptDto::createShared();
        exampleRecipe->name = "testing|lights_on";
        exampleRecipe->content = "main:\n    print \"start\"\n    illumination 0.5 0.5 0.5 0.5\n    display \"lights on\"\n    print \"done\"\n";
        info->addResponse<Object<ScriptDto>>(Status::CODE_200, "application/json", "Currently scheduled recipe")
            .addExample("application/json", exampleRecipe);
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Recipe not found")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Recipe not found"}}));
    }
    ADD_CORS(getRecipe)
    ENDPOINT("GET", "/scheduler/recipe", getRecipe);


    /**
     * @brief Select recipe to be run by the scheduler.
     */
    ENDPOINT_INFO(selectRecipe) {
        info->summary = "Assign recipe to scheduler";
        info->addTag("Scheduler");
        info->description = "Select the recipe to be run by the scheduler.";
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Recipe selected successfully")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Recipe selected successfully"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Recipe not found")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Recipe not found"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_409, "application/json", "A recipe is currently running")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to select script: Cannot change recipe while a script is running"}}));
    }
    ADD_CORS(selectRecipe)
    ENDPOINT("POST", "/scheduler/recipe/{recipeName}", selectRecipe, PATH(String, recipeName));



    /**
     * @brief Starts the scheduler.
     */
    ENDPOINT_INFO(startScheduler) {
        info->summary = "Start scheduler";
        info->addTag("Scheduler");
        info->description = "Starts the scheduler. The scheduler will run the uploaded script.";

        auto exampleProcessId = ScriptProcessIdDto::createShared();
        exampleProcessId->processId = 12345;
        info->addResponse<Object<ScriptProcessIdDto>>(Status::CODE_200, "application/json", "Scheduler started successfully")
            .addExample("application/json", exampleProcessId);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to start scheduler")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to start scheduler"}}));
    }
    ADD_CORS(startScheduler)
    ENDPOINT("POST", "/scheduler/start", startScheduler);

    /**
     * @brief Stops the scheduler.
     */
    ENDPOINT_INFO(stopScheduler) {
        info->summary = "Stop scheduler";
        info->addTag("Scheduler");
        info->description = "Stops the scheduler. The scheduler will stop running the script.";
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Scheduler stopped successfully")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Scheduler stopped successfully"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to stop scheduler")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to stop scheduler"}}));
    }
    ADD_CORS(stopScheduler)
    ENDPOINT("POST", "/scheduler/stop", stopScheduler);

    /**
     * @brief Retrieves the runtime info of the scheduler.
     */
    ENDPOINT_INFO(getSchedulerInfo) {
        info->summary = "Get scheduler info";
        info->addTag("Scheduler");
        info->description = "Retrieves the runtime info of the scheduler.";

        auto exampleInfo = ScriptRuntimeInfoDto::createShared();
        exampleInfo->processId = 12345;
        exampleInfo->name = "testing|lights_on";
        exampleInfo->started = true;
        exampleInfo->stopped = false;
        exampleInfo->startedAt = "2026-02-24 10:30:00";
        exampleInfo->finalMessage = "";
        exampleInfo->stack = oatpp::Vector<Int32>::createShared();
        exampleInfo->stack->push_back(10);
        exampleInfo->output = oatpp::Vector<String>::createShared();
        exampleInfo->output->push_back("start");
        exampleInfo->output->push_back("done");
        info->addResponse<Object<ScriptRuntimeInfoDto>>(Status::CODE_200, "application/json", "Scheduler runtime information")
            .addExample("application/json", exampleInfo);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve scheduler info")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve scheduler info"}}));
    }
    ADD_CORS(getSchedulerInfo)
    ENDPOINT("GET", "/scheduler/runtime", getSchedulerInfo);

private:
    std::shared_ptr<IScheduler> scheduler_;
    std::shared_ptr<IRecipes> recipes_;

};

#include OATPP_CODEGEN_END(ApiController)
