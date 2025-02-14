#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <SMBR/APIScheduler.hpp>

//handle sig int
#include <signal.h>
#include <atomic>

std::atomic_bool running = true;

void sigintHandler(int sig) {
    running = false;
}

int main(int argc, char* argv[]) {
    try {
        if (argc < 2) {
            std::cerr << "Usage: " << argv[0] << " <script_file> [<host>]\n";
            return 1;
        }

        std::string host = "127.0.0.1";
        if (argc > 2) {
            host = argv[2];
        }

        //register signal handler
        signal(SIGINT, sigintHandler);
        
        std::string scriptFile = argv[1];

        std::cout << "Starting script runner with script file: " << scriptFile << " and host: " << host << std::endl;

        APIClientParams params;
        params.host = host;
        APIScheduler scheduler(params);

        scheduler.setScriptFromFile(argv[1]);

        auto pid = scheduler.start();
        std::cout << "started as PID: " << pid << std::endl;

        std::string lastInfo;
        
        Poco::Timestamp start;

        while (running){
            auto info = scheduler.getRuntimeInfo();

            

            std::stringstream ss;
            ss << info;

            std::string newInfo = ss.str();

            if (newInfo != lastInfo) {
                if (info.processId == pid) {
                    std::cout << newInfo << std::endl;
                } else {
                    std::cout << "different process pending: " << newInfo << std::endl;
                }   
                
                lastInfo = newInfo;
            }

            if (info.processId == pid && info.stopped) {
                break;
            }
            if (info.processId != pid && start.elapsed() > 10 * 1000 * 1000) {
                throw std::runtime_error("Script did not start in 10 seconds, maybe there is already other one");                
            }

            //sleep 100ms
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        try {
            scheduler.stop();
        } catch (std::exception & e) {
            std::cerr << "STOP EXCEPTION: " << e.what() << std::endl;
        }

    } catch (std::exception & e) {
        std::cerr << "EXCEPTION: " << e.what() << std::endl;
    }
    
    return 0;
}
