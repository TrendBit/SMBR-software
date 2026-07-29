#pragma once

#include "oatpp/macro/component.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/data/mapping/ObjectMapper.hpp"

#include "dto/MessageDto.hpp"
#include "dto/ModuleEnum.hpp"

#include "SMBR/ISystemModule.hpp"

#include <functional>
#include <memory>
#include <string>

/**
 * @class SMBRControllerBase
 * @brief Common base of all SMBR API controllers.
 *
 * Holds the system module and the helpers shared by all controllers (uniform
 * exception handling, module lookup, enum to string conversions).
 */
class SMBRControllerBase : public oatpp::web::server::api::ApiController {
public:
    SMBRControllerBase(const std::shared_ptr<oatpp::web::mime::ContentMappers>& apiContentMappers,
                       std::shared_ptr<ISystemModule> systemModule);

    static std::string moduleToString(Modules module);

protected:
    /**
     * @brief Runs the endpoint body and converts thrown exceptions to error responses.
     */
    std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> process(
        std::string name,
        std::function<std::shared_ptr<oatpp::web::protocol::http::outgoing::Response>()> body);

    /**
     * @brief Same as process(), but the body returns success/failure instead of a response.
     */
    std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> processBool(
        std::string name,
        std::function<bool()> body);

    std::shared_ptr<ICommonModule> getModule(const oatpp::Enum<dto::ModuleEnum>::AsString& module);

    std::string instanceToString(Instance instance);

    std::shared_ptr<ISystemModule> systemModule;
};
