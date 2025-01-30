#pragma once
#include <sstream>
#include <iostream>
#include <future>
#include <chrono>
#include <string>


class LoggingProxyTransformation {

public:


    template <class T>
    std::future<T> transform(Modules module, std::string method, std::future <T> from){
        //measure time
        auto timestamp = std::chrono::system_clock::now();

        auto shared = from.share();    
        return std::async(std::launch::async, [module, method, shared, timestamp]() {
            std::stringstream s;
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