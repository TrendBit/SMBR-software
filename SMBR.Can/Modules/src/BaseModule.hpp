#pragma once

#include "codes/codes.hpp"
#include "SMBR/SMBR.hpp"
#include "SMBR/Log.hpp"
#include "can/CanChannel.hpp"
#include <iostream>
#include <deque>
#include <future>

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
            return get<Request, Response, Result>(rawRequest, interpret, timeoutMs);    
        }

        static std::string requestLogStr(CanID reqId);

        template <class Request, class Response, class Result>
        std::future <Result> get(Request rawRequest, std::function <Result(Response)> interpret, int timeoutMs){     
            Response rt;
            auto requestId = createRequestId(rawRequest.Type(), Codes::Instance::Exclusive, false);
            auto responseId = createRequestId(rt.Type(), Codes::Instance::Exclusive, false);
            
            RequestData requestData(requestId, rawRequest.Export_data());
            ResponseInfo responseInfo(responseId, timeoutMs);
            CanRequest canRequest(requestData, responseInfo);
        
            std::shared_ptr <std::promise <Result>> promise = std::make_shared<std::promise <Result>>();
            channel->send(canRequest, [&, promise, interpret](ICanChannel::Response response){
                try {
                    if (response.status == CanRequestStatus::Success) {
                        Response resp;
                        auto dataCopy = response.responseData.at(0).data;
                        if (resp.Interpret_data(dataCopy)) {
                            if (interpret){
                                auto val = interpret(resp);
                                promise->set_value(val);
                                
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
                } catch (...){
                    LERROR("Module") << "CAN get catchAll exception ..." << LE;
                    promise->set_exception(std::current_exception());
                    
                }
            });
            return promise->get_future();
        }

        template <class Request, class Response, class Result>
        std::future<Result> getWithSeq(Request rawRequest, std::function<Result(Response)> interpret, int timeoutMs, uint8_t sequenceNumber) {
            Response rt;
            auto requestId = createRequestId(rawRequest.Type(), Codes::Instance::Exclusive, false);
            auto responseId = createRequestId(rt.Type(), Codes::Instance::Exclusive, false);

            RequestData requestData(requestId, rawRequest.Export_data());
            ResponseInfo responseInfo(responseId, timeoutMs);

            responseInfo.acceptFunction = [sequenceNumber](const ResponseData& response) {
                Response resp;
                auto dataCopy = response.data; 
                if (resp.Interpret_data(dataCopy)) {
                    return resp.sequence_number == sequenceNumber;
                }
                return false;
            };

            CanRequest canRequest(requestData, responseInfo);

            std::shared_ptr<std::promise<Result>> promise = std::make_shared<std::promise<Result>>();
            channel->send(canRequest, [&, promise, interpret](ICanChannel::Response response) {
                try {
                    if (response.status == CanRequestStatus::Success) {
                        Response resp;
                        auto dataCopy = response.responseData.at(0).data; 
                        if (resp.Interpret_data(dataCopy)) {
                            if (interpret) {
                                auto val = interpret(resp);
                                promise->set_value(val);
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
                } catch (...){
                    LERROR("Module") << "CAN getWithSeq catchAll exception ..." << LE;
                    promise->set_exception(std::current_exception());
                }    
            });

            return promise->get_future();
        }

        void setMultiple(std::shared_ptr <std::promise <bool>> promise, std::deque <CanRequest> requests){
            try {
                if (requests.empty()){
                    promise->set_value(true);
                    return;
                }
                auto request = requests.front();
                requests.pop_front();
                
                channel->send(request, [&, promise, requests](ICanChannel::Response response) {
                    if (response.status == CanRequestStatus::Success) {
                        setMultiple(promise, requests);
                    } else if (response.status == CanRequestStatus::Timeout) {
                        promise->set_exception(std::make_exception_ptr(std::runtime_error("Timeout")));
                    } else {
                        promise->set_exception(std::make_exception_ptr(std::runtime_error("Failed to send request")));
                    }
                });
            } catch (...){
                LERROR("Module") << "CAN setMultiple catchAll exception ..." << LE;
                promise->set_exception(std::current_exception());
            }
        }

        template <class Request>
        CanRequest createRequest(Request rawRequest){
            auto requestId = createRequestId(rawRequest.Type(), Codes::Instance::Exclusive, false);
            RequestData requestData(requestId, rawRequest.Export_data());
            CanRequest canRequest(requestData);
            return canRequest;
        }

        template <class Request>
        std::future <bool> set(Request rawRequest){     
            std::shared_ptr <std::promise <bool>> promise = std::make_shared<std::promise <bool>>();
            CanRequest canRequest = createRequest<Request>(rawRequest);
           
            setMultiple(promise, {canRequest});
            return promise->get_future();
        }

        template <class Request, class RawResponse, class Result>
        std::future<Result> getByChannel(int8_t channelNumber,
                                        int timeoutMs,
                                        std::function<Result(const RawResponse&)> interpret) {
            auto promise = std::make_shared<std::promise<Result>>();
            
            Request rawRequest(channelNumber);
            auto requestId = createRequestId(rawRequest.Type(), Codes::Instance::Exclusive, false);
            
            RequestData requestData(requestId, rawRequest.Export_data());
            ResponseInfo responseInfo;
            responseInfo.timeoutMs = timeoutMs;

            responseInfo.acceptFunction = [channelNumber](const ResponseData& response) {
                RawResponse rawResponse;
                auto dataCopy = response.data;
                if (rawResponse.Interpret_data(dataCopy)) {
                    return response.id.messageType() == static_cast<uint16_t>(rawResponse.Type()) &&
                        static_cast<int8_t>(rawResponse.channel) == channelNumber;
                }
                return false;
            };

            responseInfo.isDoneFunction = [](const std::vector<ResponseData>& responses) {
                return !responses.empty(); 
            };

            responseInfo.successOnTimeout = false;
            CanRequest canRequest(requestData, responseInfo);

            channel->send(canRequest, [promise, channelNumber, interpret](ICanChannel::Response response) {
                try {
                    if (response.status == CanRequestStatus::Success) {
                        for (const auto& rr : response.responseData) {
                            RawResponse rawResponse;
                            auto dataCopy = rr.data;
                            if (rawResponse.Interpret_data(dataCopy) &&
                                static_cast<int8_t>(rawResponse.channel) == channelNumber) {
                                auto result = interpret(rawResponse);
                                promise->set_value(result);
                                return;
                            }
                        }
                        promise->set_exception(std::make_exception_ptr(std::runtime_error("No valid response data received")));
                    } else if (response.status == CanRequestStatus::Timeout) {
                        promise->set_exception(std::make_exception_ptr(std::runtime_error("Timeout")));
                    } else {
                        promise->set_exception(std::make_exception_ptr(std::runtime_error("Failed to send request")));
                    }
                }
                catch (...) {
                    LERROR("Module") << "CAN setMultiple catchAll exception ..." << LE;
                    promise->set_exception(std::current_exception());
                }
            });

            return promise->get_future();
        }

    private:
        ModuleID id_;
        ICanChannel::Ptr channel;
        
};
