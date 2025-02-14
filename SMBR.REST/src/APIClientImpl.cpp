#include "APIClientImpl.hpp"
#include "oatpp/macro/component.hpp"

APIClientImpl::APIClientImpl(APIClientParams params){
    
    om = std::make_shared<oatpp::json::ObjectMapper>();
    requestExecutor = createOatppExecutor();
    controllerClient = SMBRControllerClient::createShared(requestExecutor, om);
}


std::shared_ptr<oatpp::json::ObjectMapper> APIClientImpl::objectMapper(){
    return om;
}


std::shared_ptr<SMBRControllerClient> APIClientImpl::client(){
    return controllerClient;
}

std::shared_ptr<oatpp::web::client::RequestExecutor> APIClientImpl::createOatppExecutor() {
    auto connectionProvider = oatpp::network::tcp::client::ConnectionProvider::createShared({params.host, params.port});
    return oatpp::web::client::HttpRequestExecutor::createShared(connectionProvider);
}
