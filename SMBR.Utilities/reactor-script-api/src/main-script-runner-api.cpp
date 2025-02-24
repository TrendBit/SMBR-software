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

        std::string scriptFile = "";
        bool stopAsync = false;
        bool startAsync = false;
        bool isHelp = false;

        std::string host = "127.0.0.1";


        for (int i = 1; i < argc; i++) {
            std::string val = argv[i];

            if (val == "--help" || val == "-h") {
                isHelp = true;
            } else if (val == "--async-start" || val == "-s") {
                startAsync = true;
            } else if (val == "--async-stop" || val == "-x") {
                stopAsync = true;
            } else if (val == "--ip" || val == "-i") {
                if (i + 1 < argc) {
                    host = argv[i + 1];
                    i++;
                } else {
                    std::cerr << "Missing value for --ip argument" << std::endl;
                    return 1;
                }
            } else {
                if (scriptFile.empty()) {
                    scriptFile = val;
                } else {
                    std::cout << "script file: " << scriptFile << std::endl;
                    std::cerr << "Unknown argument: " << val << std::endl;
                    return 1;
                }
            } 
        }

        if (isHelp) {
            std::cout << "Usage: " << argv[0] << " <script_file>\n";
            std::cout << "Options:\n";
            std::cout << "  --help, -h - prints this help\n";
            std::cout << "  --ip, -i - set the host ip (by default it is 127.0.0.1)\n";
            std::cout << "  --async-start, -s - start the script on background\n";
            std::cout << "  --async-stop, -x - stop the script on background\n";
            return 0;
        }

        if (!stopAsync && scriptFile.empty()) {
            std::cerr << "Missing script file argument" << std::endl;
            return 1;
        }


        
        APIClientParams params;
        params.host = host;
        APIScheduler scheduler(params);


        if (stopAsync){
            std::cout << "Stopping script" << std::endl;
            scheduler.stop();
            return 0;
        }

        if (startAsync){
            std::cout << "Starting script " << scriptFile << " (" << host << ")" << std::endl;
            scheduler.setScriptFromFile(scriptFile);

            auto pid = scheduler.start();
            std::cout << "Started as PID: " << pid << " on background" << std::endl;
            return 0;
        }


        //register signal handler
        signal(SIGINT, sigintHandler);


        std::cout << "Starting script " << scriptFile << " (" << host << ")" << std::endl;

        std::cout << "Starting script " << scriptFile << " (" << host << ")" << std::endl;
        scheduler.setScriptFromFile(scriptFile);

        auto pid = scheduler.start();
        std::cout << "Started as PID: " << pid << " on foreground" << std::endl;

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
