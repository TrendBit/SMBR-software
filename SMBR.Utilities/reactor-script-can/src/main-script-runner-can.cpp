#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <stack>
#include <memory>
#include <list>
#include <stdexcept>
#include <functional>
#include <future>
#include <chrono>
#include <thread>

#include <SMBR/Scheduler.hpp>
#include "SMBR/Log.hpp"
#include "SMBR/VirtualModulesFactory.hpp"
#include "SMBR/CanModulesFactory.hpp"

#include <Poco/Logger.h>
#include <Poco/ConsoleChannel.h>


int main(int argc, char* argv[]) {
/*

  */  
    try {
        if (argc < 2) {
            std::cerr << "Usage: " << argv[0] << " <script_file> [--debug] [--virtual]\n";
            return 1;
        }

        bool debugMode = false;    
        bool isVirtual = false;

        for (int a = 2; a < argc; a++){
            if (std::string(argv[a]) == "--debug") {
                debugMode = true;
            }
            if (std::string(argv[a]) == "--virtual") {
                isVirtual = true;
            }
        }

        if (debugMode){
            //init poco logger to ConsoleLogger
            Poco::Logger & logger = Poco::Logger::get("");
            logger.setLevel(Poco::Message::PRIO_TRACE);
            Poco::AutoPtr<Poco::ColorConsoleChannel> pChannel(new Poco::ColorConsoleChannel());
            logger.setChannel(pChannel);
        }

        std::shared_ptr<ISystemModule> systemModule = nullptr;


        if (isVirtual){
            systemModule = VirtualModulesFactory::create();
        } else {
            systemModule = CanModulesFactory::create();
        }

        //sleep for 1s
        std::this_thread::sleep_for(std::chrono::seconds(1));

        Scheduler scheduler(systemModule);

        LINFO("Scheduler") << "Scheduler created" << LE;

        scheduler.setScriptFromFile(argv[1]);

        LINFO("Scheduler") << "Script assigned: " << argv[1] << LE;

        scheduler.start();


        LINFO("Scheduler") << "Script started: " << argv[1] << LE;

        while (true){
            auto info = scheduler.getRuntimeInfo();

            std::cout << info << std::endl;

            if (info.stopped) {
                break;
            }

            //sleep
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

    } catch (std::exception & e) {
        std::cerr << "EXCEPTION: " << e.what() << std::endl;
    }
    
    return 0;
}
