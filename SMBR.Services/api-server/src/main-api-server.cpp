#include <iostream>

#include "SMBR/SMBRServer.hpp"
#include "SMBR/ISystemModule.hpp"

#include "SMBR/VirtualModulesFactory.hpp"
#include "SMBR/CanModulesFactory.hpp"

#include "SMBR/Proxy/ProxySystemModule.hpp"
#include "SMBR/Proxy/Transform.hpp"

int main(int argc, char ** argv){

    try {

    system("ip link set can0 up type can bitrate 500000");

    bool isVirtual = false;

    if (argc > 1) {
        if (std::string(argv[1]) == "--virtual") {
            isVirtual = true;
        }
    }

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