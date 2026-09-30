#include <SMBR/SMBRServer.hpp>
#include <SMBR/Log.hpp>
#include "AppComponent.hpp"

#include "controllers/SystemController.hpp"
#include "controllers/ServicesController.hpp"
#include "controllers/CommonController.hpp"
#include "controllers/CoreController.hpp"
#include "controllers/ControlController.hpp"
#include "controllers/SensorController.hpp"
#include "controllers/PumpsController.hpp"
#include "controllers/RecipesController.hpp"
#include "controllers/SchedulerController.hpp"
#include "controllers/ManagerController.hpp"

#include "SMBR/Recipes.hpp"
#include "SMBR/Scheduler.hpp"

#include "oatpp/network/Server.hpp"
#include "oatpp-swagger/Controller.hpp"

#include <vector>


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

    auto scheduler = std::make_shared<Scheduler>(systemModule);
    auto recipes = std::make_shared<Recipes>("/data/recipes/", "/home/reactor/recipes/");

    std::vector<std::shared_ptr<oatpp::web::server::api::ApiController>> controllers = {
        std::make_shared<SystemController>(contentMappers, systemModule),
        std::make_shared<ServicesController>(contentMappers, systemModule),
        std::make_shared<CommonController>(contentMappers, systemModule),
        std::make_shared<CoreController>(contentMappers, systemModule),
        std::make_shared<ControlController>(contentMappers, systemModule),
        std::make_shared<SensorController>(contentMappers, systemModule),
        std::make_shared<PumpsController>(contentMappers, systemModule),
        std::make_shared<RecipesController>(contentMappers, systemModule, recipes),
        std::make_shared<SchedulerController>(contentMappers, systemModule, scheduler, recipes),
        std::make_shared<ManagerController>(contentMappers, systemModule)
    };

    oatpp::web::server::api::Endpoints docEndpoints;
    for (auto & controller : controllers) {
        router->addController(controller);
        docEndpoints.append(controller->getEndpoints());
    }

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
