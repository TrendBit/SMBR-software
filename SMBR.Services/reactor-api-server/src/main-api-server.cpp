#include <iostream>

#include "SMBR/SMBRServer.hpp"
#include "SMBR/ISystemModule.hpp"

#include "SMBR/VirtualModulesFactory.hpp"
#include "SMBR/CanModulesFactory.hpp"

#include "SMBR/Proxy/ProxySystemModule.hpp"
#include "SMBR/Proxy/Transform.hpp"

#include "SMBR/Log.hpp"

#include <chrono>
#include <thread>

#include <Poco/Util/Application.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/HelpFormatter.h>
#include <Poco/Util/IniFileConfiguration.h>

#include <SMBR/Log.hpp>

using namespace Poco;
using namespace Poco::Util;


class MyApp : public Application {
protected:
    void initialize(Application& self) override {

        // Load default configuration
        try {
            loadConfiguration("/etc/reactor/reactor-api-server.properties");
        } catch (...) {
            std::cerr << "No default configuration found.\n";
        }
        // Load and override with custom configuration (if exists)
        try {
            loadConfiguration("/data/config/reactor-api-server.properties"); 
        } catch (...) {
            std::cout << "No custom configuration found, using default.\n";
        }

        Application::initialize(self);
        LNOTICE("APP") << "--------------------------" << LE;
        LNOTICE("APP") << "reactor-api-server started" << LE;
    }

    void uninitialize() override {
        LNOTICE("APP") << "reactor-api-server stopped" << LE;
        Application::uninitialize();
    }

    int main(const std::vector<std::string>& args) override {
        try {

            //system("ip link set can0 up type can bitrate 500000");

            
            //take it from properties file
            bool isVirtual = config().getBool("server.virtual", false);
                
/*
            for (int arg = 1; arg < argc; arg++){
                if (std::string(argv[arg]) == "--virtual") {
                    isVirtual = true;
                }
            }
*/
            //SMBR::initLogs("reactor-api-server", logLevel, "/run/user/");

            std::shared_ptr<ISystemModule> systemModule = nullptr;


            if (isVirtual){
                systemModule = VirtualModulesFactory::create();
            } else {
                systemModule = CanModulesFactory::create();
            }

            //std::shared_ptr<ISystemModule> proxy = std::make_shared <ProxySystemModule<LoggingProxyTransformation> >(systemModule);

            SMBRServer server(systemModule);
            server.run();

        } catch (std::exception & e){
            LERROR("APP") << "reactor-api-server failed: " << e.what() << LE;
        }
        return Application::EXIT_OK;
    }

public:
    MyApp() {}
};

int main(int argc, char** argv) {
    MyApp app;
    app.init(argc, argv);
    return app.run();
}
