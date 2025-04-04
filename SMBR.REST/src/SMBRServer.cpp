#include <SMBR/SMBRServer.hpp>
#include <SMBR/Log.hpp>
#include "SMBRController.hpp"
#include "AppComponent.hpp"

#include "oatpp/network/Server.hpp"
#include "oatpp-swagger/Controller.hpp"


SMBRServer::SMBRServer(std::shared_ptr<ISystemModule> systemModule) : systemModule(systemModule)
{
}

SMBRServer::~SMBRServer()
{
}

static void run()
{
}

void SMBRServer::run()
{
    oatpp::Environment::init();

    AppComponent components;

    OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);
    OATPP_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>, contentMappers);

    auto controller = std::make_shared<SMBRController>(
        contentMappers, systemModule);
    router->addController(controller);

    auto docEndpoints = controller->getEndpoints();
    auto swaggerController = oatpp::swagger::Controller::createShared(docEndpoints);
    router->addController(swaggerController);

    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, connectionHandler);
    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, connectionProvider);

    oatpp::network::Server server(connectionProvider, connectionHandler);

    LNOTICE("API") << "Server running on port: " << *connectionProvider->getProperty("port").toString() << LE;
    
    server.run();

    LNOTICE("API") << "Server stopped" << LE;

    oatpp::Environment::destroy();
}
