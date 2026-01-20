#pragma once

#include "oatpp/web/server/handler/ErrorHandler.hpp"
#include "oatpp/web/protocol/http/outgoing/ResponseFactory.hpp"
#include "oatpp/web/protocol/http/outgoing/BufferBody.hpp"
#include "dto/MessageDto.hpp"

/**
 * @class CustomErrorHandler
 * @brief Custom error handler that handles API errors and returns consistent JSON responses.
 */
class CustomErrorHandler : public oatpp::web::server::handler::ErrorHandler {
private:
    typedef oatpp::web::protocol::http::outgoing::Response OutgoingResponse;
    typedef oatpp::web::protocol::http::Status Status;
    
    std::shared_ptr<oatpp::data::mapping::ObjectMapper> m_objectMapper;

public:
    CustomErrorHandler(const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper)
        : m_objectMapper(objectMapper)
    {}

    /**
     * @brief Handles errors from exceptions and converts them to appropriate HTTP responses.
     * 
     * @param exceptionPtr The exception pointer.
     * @return The outgoing response with error details.
     */
    std::shared_ptr<OutgoingResponse> handleError(const std::exception_ptr& exceptionPtr) override {
        auto error = MessageDto::createShared();
        
        try {
            if (exceptionPtr) {
                std::rethrow_exception(exceptionPtr);
            }
        } catch (const std::exception& e) {
            std::string message = e.what();
            if (message.find("Node is NOT") != std::string::npos) {
                error->message = "Invalid request body: type mismatch in field.";
                auto jsonBody = m_objectMapper->writeToString(error);
                auto response = OutgoingResponse::createShared(
                    Status::CODE_400, 
                    oatpp::web::protocol::http::outgoing::BufferBody::createShared(jsonBody)
                );
                response->putHeader("Content-Type", "application/json");
                return response;
            }
            
            if (message.find("AnyMapper") != std::string::npos || 
                message.find("no mapping") != std::string::npos) {
                error->message = "Method not allowed or endpoint not found.";
                auto jsonBody = m_objectMapper->writeToString(error);
                auto response = OutgoingResponse::createShared(
                    Status::CODE_405,
                    oatpp::web::protocol::http::outgoing::BufferBody::createShared(jsonBody)
                );
                response->putHeader("Content-Type", "application/json");
                return response;
            }
            
            if (message.find("TreeToObjectMapper") != std::string::npos ||
                message.find("Error processing request") != std::string::npos) {
                error->message = "Invalid request body: malformed JSON or unexpected structure.";
                auto jsonBody = m_objectMapper->writeToString(error);
                auto response = OutgoingResponse::createShared(
                    Status::CODE_400,
                    oatpp::web::protocol::http::outgoing::BufferBody::createShared(jsonBody)
                );
                response->putHeader("Content-Type", "application/json");
                return response;
            }
            
            error->message = message;
            auto jsonBody = m_objectMapper->writeToString(error);
            auto response = OutgoingResponse::createShared(
                Status::CODE_500,
                oatpp::web::protocol::http::outgoing::BufferBody::createShared(jsonBody)
            );
            response->putHeader("Content-Type", "application/json");
            return response;
        }
        
        error->message = "An error occurred.";
        auto jsonBody = m_objectMapper->writeToString(error);
        auto response = OutgoingResponse::createShared(
            Status::CODE_500,
            oatpp::web::protocol::http::outgoing::BufferBody::createShared(jsonBody)
        );
        response->putHeader("Content-Type", "application/json");
        return response;
    }
};
