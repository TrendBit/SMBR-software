#pragma once

#include "SMBR/APIClient.hpp"
#include "SMBRControllerClient.hpp"
#include "oatpp/web/client/HttpRequestExecutor.hpp"
#include "oatpp/network/tcp/client/ConnectionProvider.hpp"
#include "oatpp/json/ObjectMapper.hpp"

class APIClientImpl {

    public:
        APIClientImpl(APIClientParams params);
        std::shared_ptr<SMBRControllerClient> client();
        std::shared_ptr<oatpp::json::ObjectMapper> objectMapper();
    private:
        std::shared_ptr<oatpp::web::client::RequestExecutor> createOatppExecutor();
    private:
        APIClientParams params;
        std::shared_ptr<oatpp::json::ObjectMapper> om;
        std::shared_ptr<oatpp::web::client::RequestExecutor> requestExecutor;
        std::shared_ptr<SMBRControllerClient> controllerClient;

};
