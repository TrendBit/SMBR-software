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

#include "SMBR/VirtualModulesFactory.hpp"
#include "SMBR/CanModulesFactory.hpp"


int main(int argc, char* argv[]) {
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

        std::shared_ptr<ISystemModule> systemModule = nullptr;


        if (isVirtual){
            systemModule = VirtualModulesFactory::create();
        } else {
            systemModule = CanModulesFactory::create();
        }

        //sleep for 1s
        std::this_thread::sleep_for(std::chrono::seconds(1));

        Scheduler scheduler(systemModule);

        scheduler.setScriptFromFile(argv[1]);

        scheduler.start();

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
