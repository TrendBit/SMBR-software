#include <iostream>

#include "SMBR/SMBRServer.hpp"
#include "SMBR/ISystemModule.hpp"

#include "SMBR/VirtualModulesFactory.hpp"

#include "SMBR/Proxy/ProxySystemModule.hpp"
#include "SMBR/Proxy/Transform.hpp"

int main(int argc, char ** argv){

    bool isVirtual = false;

    if (argc > 1) {
        if (std::string(argv[1]) == "--virtual") {
            isVirtual = true;
        }
    }

    std::shared_ptr<ISystemModule> systemModule = nullptr;


    //if (isVirtual){
    systemModule = VirtualModulesFactory::create();
    //}


    std::shared_ptr<ISystemModule> proxy = std::make_shared <ProxySystemModule<LoggingProxyTransformation> >(systemModule);

    SMBRServer server(proxy);
    server.run();

    return 0;
}