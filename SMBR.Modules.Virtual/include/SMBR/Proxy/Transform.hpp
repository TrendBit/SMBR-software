#pragma once
#include <sstream>
#include <iostream>
#include <future>
#include <chrono>
#include <string>
#include <iomanip>

class LoggingProxyTransformation {

public:
    static std::string number(int num, int length){
        std::stringstream s;
        s << std::setfill('0') << std::setw(length) << num;
        return s.str();
    }

    template <class T>
    std::future<T> transform(Modules module, std::string method, std::future <T> from){
        //measure time
        auto timestamp = std::chrono::system_clock::now();

        auto shared = from.share();    
        return std::async(std::launch::async, [module, method, shared, timestamp]() {
            std::stringstream s;

            //write minutes, seconds and ms of local datetime HH:MM:SS.mmm
            {
                auto now = std::chrono::system_clock::now();
                auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
                auto timer = std::chrono::system_clock::to_time_t(now);
                auto localTime = std::localtime(&timer);
                //print with leading zeros
                s << number(localTime->tm_hour,2) << ":" << number(localTime->tm_min,2) << ":" << number(localTime->tm_sec, 2) << "." << number(ms.count(), 3) << " | ";
            }    

            {
                std::stringstream sm;
                sm << module;
                s << align(sm.str(), 8) << " | " << align(method, 30) << " | ";
            }
            
            try {
                shared.wait();
                auto value = shared.get();

                {
                    std::stringstream ts;
                    ts << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - timestamp).count() << "ms";
                    s << align(ts.str(), 7, false) << " | ";
                }
                {
                    std::stringstream ts;
                    ts << value;
                    s << align(ts.str(), 20, false) << " | ";
                }
                std::cout << s.str() << std::endl;
                return value;
            } catch (...){
                {
                    std::stringstream ts;
                    ts << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - timestamp).count() << "ms";
                    s << align(ts.str(), 7, false) << " | ";
                }
                std::cout << s.str() << " | FAILED"  << std::endl;
                throw;
            }
            
            
        });
    }
        
private:
    static std::string align(std::string message, int length, bool fromLeft = true){
        std::string fmessage = message;
        if (message.length() > length){
            fmessage = message.substr(0, length - 3) + "...";
        }
        if (message.length() < length){
            if (!fromLeft){ 
                fmessage = std::string(length - message.length(), ' ') + message;
            } else {
                fmessage = message + std::string(length - message.length(), ' ');
            }
        }
         return fmessage;
    }

    
};