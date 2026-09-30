#pragma once

#include "BaseController.hpp"

#include "dto/ManagedDto.hpp"
#include "dto/MessageDto.hpp"

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
                      std::shared_ptr<ISystemModule> systemModule);

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

private:
    struct Flag {
        bool active = false;
        std::string managerId;
    };

    mutable std::mutex mutex_;
    Flag managed_;
};

#include OATPP_CODEGEN_END(ApiController)
