#pragma once

#include "BaseController.hpp"

#include "dto/MessageDto.hpp"
#include "dto/ServiceEnum.hpp"
#include "dto/ServiceLogsDto.hpp"
#include "dto/ServiceStatusDto.hpp"

#include <string>
#include <vector>

#include OATPP_CODEGEN_BEGIN(ApiController)

#undef OATPP_MACRO_API_CONTROLLER_ADD_CORS_BODY_DEFAULT_METHODS
#define OATPP_MACRO_API_CONTROLLER_ADD_CORS_BODY_DEFAULT_METHODS "GET, POST, OPTIONS, PUT, PATCH, DELETE"

/**
 * @class ServicesController
 * @brief Endpoints controlling the systemd services of the reactor.
 */
class ServicesController : public SMBRControllerBase {
public:
    ServicesController(const std::shared_ptr<oatpp::web::mime::ContentMappers>& apiContentMappers,
                       std::shared_ptr<ISystemModule> systemModule);

    /**
     * @brief Retrieves the systemd status of every managed SMBR service.
     */
    ENDPOINT_INFO(getServiceStatuses) {
        info->summary = "List status of all managed SMBR services";
        info->addTag("Services");
        info->description = "Returns the systemd status of every managed service.";
        auto example = oatpp::List<Object<ServiceStatusDto>>::createShared();
        auto core = ServiceStatusDto::createShared();
        core->name = "reactor-core-module.service";
        core->load_state = "loaded";
        core->active_state = "active";
        core->sub_state = "running";
        core->enabled = true;
        core->main_pid = 1234;
        core->since = "Wed 2026-07-15 08:30:12 UTC";
        example->push_back(core);
        auto db = ServiceStatusDto::createShared();
        db->name = "reactor-database-export.service";
        db->load_state = "loaded";
        db->active_state = "failed";
        db->sub_state = "failed";
        db->enabled = true;
        db->main_pid = 0;
        db->since = "Wed 2026-07-15 08:31:47 UTC";
        example->push_back(db);
        info->addResponse<List<Object<ServiceStatusDto>>>(Status::CODE_200, "application/json", "Status of all managed services")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to query the service via systemctl")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve status: systemctl exited with code 1"}}));
    }
    ADD_CORS(getServiceStatuses)
    ENDPOINT("GET", "/services", getServiceStatuses);

    /**
     * @brief Retrieves the current systemd status of the given managed service.
     */
    ENDPOINT_INFO(getServiceStatus) {
        info->summary = "Get status of a single managed service";
        info->addTag("Services");
        info->description = "Returns the current systemd status of the given service unit.";
        auto example = ServiceStatusDto::createShared();
        example->name = "reactor-core-module.service";
        example->load_state = "loaded";
        example->active_state = "active";
        example->sub_state = "running";
        example->enabled = true;
        example->main_pid = 1234;
        example->since = "Wed 2026-07-15 08:30:12 UTC";
        info->addResponse<Object<ServiceStatusDto>>(Status::CODE_200, "application/json", "Service status")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Service unit not found")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Unit 'reactor-core-module.service' not found"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to query the service via systemctl")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve status: systemctl exited with code 1"}}));
    }
    ADD_CORS(getServiceStatus)
    ENDPOINT("GET", "/services/{service}", getServiceStatus, PATH(oatpp::Enum<dto::ServiceEnum>::AsString, service));

    /**
     * @brief Retrieves recent journal log lines for the given managed service.
     */
    ENDPOINT_INFO(getServiceLogs) {
        info->summary = "Get recent log lines for a service";
        info->addTag("Services");
        info->description = "Returns the most recent journal entries for the given service unit.";
        info->queryParams.add<oatpp::Int32>("lines").required = false;
        info->queryParams["lines"].description = "Number of most recent log lines to return (1-1000, default 100).";

        auto example = ServiceLogsDto::createShared();
        example->name = "reactor-core-module.service";
        example->lines = oatpp::List<oatpp::String>::createShared();
        example->lines->push_back("Jul 15 08:30:12 reactor reactor-core-module[1234]: Starting SMBR Core Module Service...");

        info->addResponse<Object<ServiceLogsDto>>(Status::CODE_200, "application/json", "Recent log lines for the service")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Invalid lines parameter")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "lines must be between 1 and 1000"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Service unit not found")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Unit 'reactor-core-module.service' not found"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to query the service logs via journalctl")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve logs: journalctl exited with code 1"}}));
    }
    ADD_CORS(getServiceLogs)
    ENDPOINT("GET", "/services/{service}/logs", getServiceLogs,
        REQUEST(std::shared_ptr<IncomingRequest>, request), PATH(oatpp::Enum<dto::ServiceEnum>::AsString, service));

    /**
     * @brief Starts the given managed service.
     */
    ENDPOINT_INFO(startService) {
        info->summary = "Start a service";
        info->addTag("Services");
        info->description = "Starts the given service unit.";
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Service started")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Successfully started reactor-web-control-ts.service"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Service unit not found")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Unit 'reactor-core-module.service' not found"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to query or act on the service via systemctl")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to start service: systemctl exited with code 1"}}));
    }
    ADD_CORS(startService)
    ENDPOINT("POST", "/services/{service}/start", startService, PATH(oatpp::Enum<dto::ServiceEnum>::AsString, service));

    /**
     * @brief Stops the given managed service.
     */
    ENDPOINT_INFO(stopService) {
        info->summary = "Stop a service";
        info->addTag("Services");
        info->description =
            "Stops the given service unit.\n\n"
            "**Caution:** stopping `api-server` or `can0` may make the device unreachable over the API/CAN bus until it is manually restarted.";
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Service stopped")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Successfully stopped reactor-web-control-ts.service"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Service unit not found")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Unit 'reactor-core-module.service' not found"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to query or act on the service via systemctl")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to stop service: systemctl exited with code 1"}}));
    }
    ADD_CORS(stopService)
    ENDPOINT("POST", "/services/{service}/stop", stopService, PATH(oatpp::Enum<dto::ServiceEnum>::AsString, service));

    /**
     * @brief Restarts the given managed service.
     */
    ENDPOINT_INFO(restartService) {
        info->summary = "Restart a service";
        info->addTag("Services");
        info->description =
            "Restarts the given service unit.";
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Service restarted")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Successfully restarted reactor-core-module.service"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Service unit not found")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Unit 'reactor-core-module.service' not found"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to query or act on the service via systemctl")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to restart service: systemctl exited with code 1"}}));
    }
    ADD_CORS(restartService)
    ENDPOINT("POST", "/services/{service}/restart", restartService, PATH(oatpp::Enum<dto::ServiceEnum>::AsString, service));

    /**
     * @brief Enables the given managed service to start at boot.
     */
    ENDPOINT_INFO(enableService) {
        info->summary = "Enable a service to start at boot";
        info->addTag("Services");
        info->description = "Enables the given service unit.";
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Service enabled")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Successfully enabled reactor-database-export.service"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Service unit not found")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Unit 'reactor-core-module.service' not found"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to query or act on the service via systemctl")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to enable service: systemctl exited with code 1"}}));
    }
    ADD_CORS(enableService)
    ENDPOINT("POST", "/services/{service}/enable", enableService, PATH(oatpp::Enum<dto::ServiceEnum>::AsString, service));

    /**
     * @brief Disables the given managed service from starting at boot.
     */
    ENDPOINT_INFO(disableService) {
        info->summary = "Disable a service from starting at boot";
        info->addTag("Services");
        info->description = "Disables the given service unit.";
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Service disabled")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Successfully disabled reactor-database-export.service"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Service unit not found")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Unit 'reactor-core-module.service' not found"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to query or act on the service via systemctl")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to disable service: systemctl exited with code 1"}}));
    }
    ADD_CORS(disableService)
    ENDPOINT("POST", "/services/{service}/disable", disableService, PATH(oatpp::Enum<dto::ServiceEnum>::AsString, service));
    
private:
    struct SystemdUnitStatus {
        std::string loadState;
        std::string activeState;
        std::string subState;
        std::string unitFileState;
        int32_t mainPid = 0;
        std::string since;
    };
    std::string serviceUnitName(const oatpp::Enum<dto::ServiceEnum>::AsString& service);
    SystemdUnitStatus querySystemdUnit(const std::string& unitName);
    oatpp::Object<ServiceStatusDto> toServiceStatusDto(const std::string& unitName, const SystemdUnitStatus& status);

    std::vector<std::string> queryServiceLogs(const std::string& unitName, int lineCount);
    void runSystemctlAction(const std::string& unitName, const std::string& action);
    std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> performServiceAction(
        const oatpp::Enum<dto::ServiceEnum>::AsString& service,
        const std::string& systemctlAction,
        const std::string& pastTenseVerb);

};

#include OATPP_CODEGEN_END(ApiController)
