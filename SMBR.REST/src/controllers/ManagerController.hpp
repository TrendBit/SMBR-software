#pragma once

#include "BaseController.hpp"

#include "dto/ControlledDto.hpp"
#include "dto/ManagedDto.hpp"
#include "dto/MessageDto.hpp"

#include "ControlledState.hpp"

#include <mutex>
#include <string>

#include OATPP_CODEGEN_BEGIN(ApiController)

#undef OATPP_MACRO_API_CONTROLLER_ADD_CORS_BODY_DEFAULT_METHODS
#define OATPP_MACRO_API_CONTROLLER_ADD_CORS_BODY_DEFAULT_METHODS "GET, POST, OPTIONS, PUT, PATCH, DELETE"

/**
 * @class ManagerController
 * @brief Endpoints reporting/asserting whether a manager sees or controls this reactor.
 */
class ManagerController : public SMBRControllerBase {
public:
    ManagerController(const std::shared_ptr<oatpp::web::mime::ContentMappers>& apiContentMappers,
                      std::shared_ptr<ISystemModule> systemModule,
                      std::shared_ptr<ControlledState> controlledState);

    ENDPOINT_INFO(getManaged) {
        info->summary = "Get whether a manager sees this reactor";
        info->addTag("Manager");
        info->description =
            "Returns whether a manager currently sees this reactor on the network, and which manager.";
        auto example = ManagedDto::createShared();
        example->manager_id = "a1b2c3d4";
        example->managed = true;
        info->addResponse<Object<ManagedDto>>(Status::CODE_200, "application/json", "Current managed state")
            .addExample("application/json", example);
    }
    ADD_CORS(getManaged)
    ENDPOINT("GET", "/manager/managed", getManaged);

    ENDPOINT_INFO(setManaged) {
        info->summary = "Set whether a manager sees this reactor";
        info->addTag("Manager");
        info->description =
            "Called by a manager to assert or clear that it sees this reactor on the network. "
            "The flag is sticky: it only changes on this call, there is no timeout.";
        auto example = ManagedDto::createShared();
        example->manager_id = "a1b2c3d4";
        example->managed = true;
        info->addConsumes<Object<ManagedDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Managed state updated")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "setManaged successful"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Invalid request body")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Invalid request body: manager_id is required"}}));
    }
    ADD_CORS(setManaged)
    ENDPOINT("POST", "/manager/managed", setManaged, BODY_DTO(Object<ManagedDto>, body));

    ENDPOINT_INFO(getControlled) {
        info->summary = "Get whether a manager actively controls this reactor";
        info->addTag("Manager");
        info->description =
            "Returns whether a manager currently actively controls this reactor, and which manager.";
        auto example = ControlledDto::createShared();
        example->manager_id = "a1b2c3d4";
        example->controlled = true;
        info->addResponse<Object<ControlledDto>>(Status::CODE_200, "application/json", "Current controlled state")
            .addExample("application/json", example);
    }
    ADD_CORS(getControlled)
    ENDPOINT("GET", "/manager/controlled", getControlled);

    ENDPOINT_INFO(setControlled) {
        info->summary = "Set whether a manager actively controls this reactor";
        info->addTag("Manager");
        info->description =
            "Called by a manager to assert or clear that it actively controls this reactor. "
            "The flag is sticky: it only changes on this call, there is no timeout.";
        auto example = ControlledDto::createShared();
        example->manager_id = "a1b2c3d4";
        example->controlled = true;
        info->addConsumes<Object<ControlledDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Controlled state updated")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "setControlled successful"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Invalid request body")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Invalid request body: manager_id is required"}}));
    }
    ADD_CORS(setControlled)
    ENDPOINT("POST", "/manager/controlled", setControlled, BODY_DTO(Object<ControlledDto>, body));

private:
    struct Flag {
        bool active = false;
        std::string managerId;
    };

    mutable std::mutex mutex_;
    Flag managed_;
    std::shared_ptr<ControlledState> controlledState_;
};

#include OATPP_CODEGEN_END(ApiController)
