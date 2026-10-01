#pragma once

#include "oatpp/web/server/interceptor/RequestInterceptor.hpp"
#include "oatpp/web/protocol/http/outgoing/BufferBody.hpp"
#include "oatpp/data/mapping/ObjectMapper.hpp"
#include "dto/MessageDto.hpp"
#include "ControlledState.hpp"

#include <set>
#include <string>

/**
 * @class ManagerControlGuard
 * @brief Rejects state-changing requests while a manager controls this reactor,
 *        unless they come from that manager (identified by the Manager-Id header).
 */
class ManagerControlGuard : public oatpp::web::server::interceptor::RequestInterceptor {
private:
    typedef oatpp::web::protocol::http::outgoing::Response OutgoingResponse;
    typedef oatpp::web::protocol::http::Status Status;

    std::shared_ptr<ControlledState> m_controlledState;
    std::shared_ptr<oatpp::data::mapping::ObjectMapper> m_objectMapper;

    static std::string pathWithoutQuery(const std::string & path) {
        return path.substr(0, path.find('?'));
    }

    /**
     * @brief GET endpoints that change reactor state despite the method, so they are guarded too.
     */
    static bool isMutatingGet(const std::string & path) {
        static const std::set<std::string> paths = {
            "/control/heater/turn_off",
            "/control/cuvette_pump/stop",
            "/control/aerator/stop",
            "/control/mixer/stop",
            "/sensor/oled/clear_custom_text",
        };
        if (paths.count(path)) {
            return true;
        }
        // /pumps/{instance_index}/stop/{pump_index}
        return path.rfind("/pumps/", 0) == 0 && path.find("/stop/") != std::string::npos;
    }

    /**
     * @brief Non-GET endpoints that are safe to call even while a manager controls the reactor.
     */
    static bool isAllowedNonGet(const std::string & path) {
        static const std::set<std::string> paths = {
            "/sensor/spectrophotometer/measure_all",
        };
        return paths.count(path) > 0;
    }

public:
    ManagerControlGuard(const std::shared_ptr<ControlledState>& controlledState,
                        const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper)
        : m_controlledState(controlledState)
        , m_objectMapper(objectMapper)
    {}

    /**
     * @brief Returns nullptr to let the request through, or a 409 response to reject it.
     */
    std::shared_ptr<OutgoingResponse> intercept(const std::shared_ptr<IncomingRequest>& request) override {
        const auto & startingLine = request->getStartingLine();
        std::string method = startingLine.method.std_str();
        std::string path = pathWithoutQuery(startingLine.path.std_str());

        if (method == "OPTIONS") {
            return nullptr;
        }
        if (path.rfind("/manager/", 0) == 0) {
            return nullptr;
        }

        auto flag = m_controlledState->get();
        if (!flag.active) {
            return nullptr;
        }

        auto managerId = request->getHeader("Manager-Id");
        if (managerId && *managerId == flag.managerId) {
            return nullptr;
        }

        if (method == "GET" && !isMutatingGet(path)) {
            return nullptr;
        }
        if (method != "GET" && isAllowedNonGet(path)) {
            return nullptr;
        }

        // Drain the unread body, otherwise on a keep-alive connection it would be parsed as the beginning of the next request.
        request->readBodyToString();

        auto error = MessageDto::createShared();
        error->message = "Reactor is currently controlled by manager " + flag.managerId;
        auto response = OutgoingResponse::createShared(
            Status::CODE_409,
            oatpp::web::protocol::http::outgoing::BufferBody::createShared(m_objectMapper->writeToString(error))
        );
        response->putHeader("Content-Type", "application/json");
        response->putHeader("Access-Control-Allow-Origin", "*");
        return response;
    }
};
