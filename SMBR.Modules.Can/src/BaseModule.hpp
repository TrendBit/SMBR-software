#pragma once

#include "codes/codes.hpp"
#include "SMBR/SMBR.hpp"
#include "can/CanChannel.hpp"

//TODO remove it
#include <iostream>

typedef uint32_t MessageID;

class BaseModule {
    public:
        BaseModule(ModuleID i, ICanChannel::Ptr channel);
        
        Modules module() const;
        std::string uidHex() const;
        ModuleID id() const;

        ICanChannel::Ptr ch(){
            return channel;
        }

        CanID createRequestId(Codes::Message_type messageType, Codes::Instance instance, bool emergencyFlag);

        static CanID createId(Codes::Message_type messageType, Modules module, Codes::Instance instance, bool emergencyFlag);
        static CanID createId(Codes::Message_type messageType, Codes::Module module, Codes::Instance instance, bool emergencyFlag);

    template <class Request, class Response, class Result>
    std::future <Result> get(std::function <Result(Response)> interpret, int timeoutMs){     

        Request rawRequest;
        Response rawResponse;
        return get<Request, Response, Result>(rawRequest, rawResponse, interpret, timeoutMs);    
    }

    template <class Request, class Response, class Result>
    std::future <Result> get(Request rawRequest, std::function <Result(Response)> interpret, int timeoutMs){     
        Response rawResponse;
        return get<Request, Response, Result>(rawRequest, rawResponse, interpret, timeoutMs);

    }

    template <class Request, class Response, class Result>
    std::future <Result> get(Request rawRequest, Response rawResponse, std::function <Result(Response)> interpret, int timeoutMs){     
        
        auto requestId = createRequestId(rawRequest.Type(), Codes::Instance::Exclusive, false);
        auto responseId = createRequestId(rawResponse.Type(), Codes::Instance::Exclusive, false);
        
        RequestData requestData(requestId, rawRequest.Export_data());
        ResponseInfo responseInfo(responseId, timeoutMs);
        CanRequest canRequest(requestData, responseInfo);
    
        std::shared_ptr <std::promise <Result>> promise = std::make_shared<std::promise <Result>>();
        channel->send(canRequest, [&, promise, interpret, rawResponse](ICanChannel::Response response){
            
            if (response.status == CanRequestStatus::Success) {
                auto rawCpy = rawResponse;
                auto dataCopy = response.responseData.at(0).data;
                if (rawCpy.Interpret_data(dataCopy)) {
                    if (interpret){
                        promise->set_value(interpret(rawCpy));
                    } else {
                        promise->set_exception(std::make_exception_ptr(std::runtime_error("No interpret call")));
                    }    
                } else {
                    promise->set_exception(std::make_exception_ptr(std::runtime_error("Failed to interpret response")));
                }
            } else if (response.status == CanRequestStatus::Timeout) {
                promise->set_exception(std::make_exception_ptr(std::runtime_error("Timeout")));
            } else {
                promise->set_exception(std::make_exception_ptr(std::runtime_error("Failed to send request")));
            }
        });
        return promise->get_future();
    }

    template <class Request>
    std::future <bool> set(Request rawRequest){     
        
        auto requestId = createRequestId(rawRequest.Type(), Codes::Instance::Exclusive, false);
        
        RequestData requestData(requestId, rawRequest.Export_data());
        CanRequest canRequest(requestData);
    
        std::shared_ptr <std::promise <bool>> promise = std::make_shared<std::promise <bool>>();
        channel->send(canRequest, [&, promise](ICanChannel::Response response){
            if (response.status == CanRequestStatus::Success) {
                promise->set_value(true);
            } else if (response.status == CanRequestStatus::Timeout) {
                promise->set_exception(std::make_exception_ptr(std::runtime_error("Timeout")));
            } else {
                promise->set_exception(std::make_exception_ptr(std::runtime_error("Failed to send request")));
            }
        });
        return promise->get_future();
    }

    private:
        ModuleID id_;
        ICanChannel::Ptr channel;
        
};
