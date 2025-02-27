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

int main(int argc, char ** argv){
    
    //sleep for 2s after start - uncomment when using better systemd order


    try {

    //system("ip link set can0 up type can bitrate 500000");

    bool isVirtual = false;
    int logLevel = 3;

    for (int arg = 1; arg < argc; arg++){
        if (std::string(argv[arg]) == "--virtual") {
            isVirtual = true;
        }
        if (std::string(argv[arg]) == "--fulllog") {
            logLevel = 8;
        }
    }

    SMBR::initLogs("reactor-api-server", logLevel, "/run/user/");

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
        std::cerr << "EXCEPTION: " << e.what() << std::endl;
    }
    return 0;
}
