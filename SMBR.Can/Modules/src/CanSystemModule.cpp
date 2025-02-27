#include "CanSystemModule.hpp"

#include <iostream>       // std::cout
#include <future>         // std::async, std::future
#include <iomanip>
#include <sstream>

#include <SMBR/Exceptions.hpp>

#include "CanCoreModule.hpp"
#include "CanControlModule.hpp"
#include "CanSensorModule.hpp"
#include "CanCommonModule.hpp"

#include "codes/codes.hpp"
#include "codes/messages/common/probe_modules_request.hpp"
#include "codes/messages/common/probe_modules_response.hpp"

CanSystemModule::CanSystemModule(ICanChannel::Ptr channel) : channel(channel) {
    this->channel = channel;

    std::lock_guard<std::mutex> lock(m);    
    refresh();


    
}

void CanSystemModule::refresh(){
    auto detected = getAvailableModules();
    detected.wait();
    auto m = detected.get();
    for (const auto& mm : m) {
        switch (mm.type) {
            case Modules::Core:
                if (!core){
                    core = std::make_shared<CanCoreModule>(mm.uidHex, channel);
                }
                break;
            case Modules::Control:
                if (!control){
                    control = std::make_shared<CanControlModule>(mm.uidHex, channel);
                }
                break;
            case Modules::Sensor:
                if (!sensor){
                    sensor = std::make_shared<CanSensorModule>(mm.uidHex, channel);
                }
                break;
            default:
                break;
        }
        common[mm] = std::make_shared<CanCommonModule>(mm, channel);
    }

    std::cout << "DETECTED MODULES: " << common.size() << std::endl;
    for (const auto& mm : common) {
        std::cout << "MODULE: " << mm.first << std::endl;
    }
}

std::future <ISystemModule::AvailableModules> CanSystemModule::getAvailableModules() {

   
    App_messages::Common::Probe_modules_request rawRequest;
    

    CanID requestId = BaseModule::createId(rawRequest.Type(), Codes::Module::All, Codes::Instance::Exclusive, false);
    
    RequestData requestData(requestId, rawRequest.Export_data());
    ResponseInfo responseInfo;
    responseInfo.timeoutMs = 300;
    responseInfo.acceptFunction = [](const ResponseData & response) {
        App_messages::Common::Probe_modules_response rawResponse;
        auto t = static_cast<uint16_t>(rawResponse.Type());
        return response.id.messageType() == t;
    };
    responseInfo.isDoneFunction = [](const std::vector <ResponseData> & responses) {
        return false;
    };
    responseInfo.successOnTimeout = true;  

    CanRequest canRequest(requestData, responseInfo);
    
    auto promise = std::make_shared<std::promise <ISystemModule::AvailableModules>>();


    channel->send(canRequest, [&, promise](ICanChannel::Response response){
            if (response.status == CanRequestStatus::Success) {

                ISystemModule::AvailableModules result;

                for (const auto& rr : response.responseData) {
                    auto dataCopy = rr.data;
                    App_messages::Common::Probe_modules_response moduleResponse;
                    if (moduleResponse.Interpret_data(dataCopy)) {
                        std::stringstream uidHex;
                        uidHex << "0x";
                        for (const auto& byte : moduleResponse.uid) {
                            uidHex << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(byte);
                        }
                        std::string uid = uidHex.str();
                        Modules module = ::Modules::Unknown;  
                        auto modulePart = rr.id.moduleAddress();
                        switch (modulePart) {
                            case CanID::Core_module:
                                module = ::Modules::Core;
                                break;
                            case CanID::Control_module:
                                module = ::Modules::Control;
                                break;
                            case CanID::Sensor_module:
                                module = ::Modules::Sensor;
                                break;
                            default:
                                break;
                        }

                        Instance instance = static_cast<Instance>(rr.id.instance());
                        ModuleID moduleID(module, uid, instance);
                        result.push_back(moduleID);
                    }
                }

                promise->set_value(result);
            } else if (response.status == CanRequestStatus::Timeout) {
                promise->set_exception(std::make_exception_ptr(std::runtime_error("Timeout")));
            } else {
                promise->set_exception(std::make_exception_ptr(std::runtime_error("Failed to send request")));
            }
    });
    return promise->get_future();   
}

std::shared_ptr <ISensorModule> CanSystemModule::sensorModule() {
    std::lock_guard<std::mutex> lock(m);    
    if (sensor == nullptr) {
        refresh();
        if (sensor == nullptr) {
            throw NotFoundException("Sensor module not available");
        }
    }
    return sensor;
}

std::shared_ptr <IControlModule> CanSystemModule::controlModule() {
    std::lock_guard<std::mutex> lock(m);    
    if (control == nullptr) {
        refresh();
        if (control == nullptr) {
            throw NotFoundException("Control module not available");
        }
    }
    return control;
}

std::shared_ptr <ICoreModule> CanSystemModule::coreModule() {
    std::lock_guard<std::mutex> lock(m);    
    if (core == nullptr) {
        refresh();
        if (core == nullptr) {
            throw NotFoundException("Core module not available");
        }
    }
    return core;
}

std::shared_ptr <ICommonModule> CanSystemModule::commonModule(ModuleID module) {
    std::lock_guard<std::mutex> lock(m);    
    try {
        if (common.find(module) == common.end()) {
            refresh();
        }
        return common.at(module);
    } catch (const std::out_of_range& e) {
        std::stringstream ss;
        ss << "Common module " << module << " not available";
        throw NotFoundException(ss.str());
    }
}

std::set <ModuleID> CanSystemModule::existing() {
    std::lock_guard<std::mutex> lock(m);    
    std::set <ModuleID>  ret;
    for (auto m : common){
        ret.insert(m.first);
    }
    return ret;
}
