#include "CanChannel.hpp"
#include "CanBus.hpp"
#include <iostream>
#include <sstream>
#include "BaseModule.hpp"
#include <Poco/Clock.h>
#include <Poco/Timespan.h>
#include <Poco/DateTime.h>
#include <Poco/LocalDateTime.h>
#include <Poco/DateTimeFormatter.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/NumberFormatter.h>
#include <Poco/Message.h>   
#include <Poco/Logger.h>   
#include "SMBR/Log.hpp"

int nextUID() {
    static int uid = 0;
    return uid++;
}

CanChannel::ActiveRequest::ActiveRequest(const CanRequest &request) : uid(nextUID()), request(request), response(request) {
}

CanChannel::CanChannel() : bgThreadRead("can.bg.read"), bgThreadWrite("can.bg.write") {
    bgThreadRead.startFunc([this]() {
        SMBR::threadDebug();
        runRead();
    }); 
    bgThreadWrite.startFunc([this]() {
        SMBR::threadDebug();
        runWrite();
    }); 
}

CanChannel::~CanChannel() {
    bgFinished = true;
    somethingToWrite.set();
    bgThreadRead.join();
    bgThreadWrite.join();
}

void CanChannel::send(const CanRequest &canRequest, std::function<void(Response)> callback) {
    auto newRequest = std::make_shared<ActiveRequest>(canRequest);
    newRequest->startedAt = Poco::Clock();
    newRequest->dueAt = newRequest->startedAt + Poco::Clock::ClockDiff(canRequest.responseInfo().timeoutMs * 1000LL);
    newRequest->callback = callback;
    {
        std::scoped_lock lock(toBeSendMutex);
        toBeSend.push_back(newRequest);
    }
    somethingToWrite.set();
}

static void dump(bool isWrite, std::string message, std::shared_ptr<CanChannel::ActiveRequest> r, Poco::Message::Priority priority = Poco::Message::PRIO_INFORMATION) {
    


    Poco::LocalDateTime dt;
    if (isWrite){
       LLEVEL("CAN.Send", priority) 
              << "Can >> "
              << " " << Poco::NumberFormatter::format0(r->uid, 5)
              << " " << r->request.request().id
              << " " << message
              << " (after " << r->startedAt.elapsed() / 1000 << "ms)" << LE;
    } else {
         LLEVEL("CAN.Recv", priority) 
              << "Can << "
              << " " << Poco::NumberFormatter::format0(r->uid, 5)
              << " " << r->request.request().id
              << " " << r->response.status
              << " " << message
              << " (after " << r->startedAt.elapsed() / 1000 << "ms)" << LE;
    }
}

void CanChannel::runWrite(){
    auto sendRequest = [&](std::shared_ptr<ActiveRequest> request)
    {
        try {
            CanMessage msg(request->request.request().id.id, request->request.request().data);
            bus.send(msg);
            dump(true, "SENT", request, Poco::Message::PRIO_DEBUG);

            {
                std::scoped_lock lock(activeRequestsMutex);
                activeRequests.push_back(request);
            }
        } catch (...) {
            dump(true, "FAILED", request, Poco::Message::PRIO_ERROR);
            request->response.status = CanRequestStatus::Fail;
            request->callback(request->response);
        }
    };

    while (!bgFinished) {
        try {
            somethingToWrite.wait();
            if (bgFinished){
                break;
            }

            std::vector<std::shared_ptr<ActiveRequest>> toSend;
            {
                std::scoped_lock lock(toBeSendMutex);
                toSend = toBeSend;
                toBeSend.clear();
            }

            for (auto s : toSend) {
                sendRequest(s);
            }
        } catch (std::exception &e) {
            LERROR("CAN") << "Failed to send CAN message (" << e.what() << ")" << LE;
        } catch (...) {
            LERROR("CAN") << "Failed to send CAN message (unknown error)" << LE;
        }
    }
}

void CanChannel::runRead() {

    while (!bgFinished) {
        try {
            
            try {
                auto rawResponse = bus.receive(100);
                ResponseData response(rawResponse.getId(), rawResponse.getData());

                bool accepted = false;
                std::scoped_lock lock(activeRequestsMutex);
                for (auto a : activeRequests) {
                    // response info
                    const ResponseInfo &responseInfo = a->request.responseInfo();

                    if (responseInfo.acceptFunction && responseInfo.acceptFunction(response)) {
                        std::stringstream ss;
                        ss << "accepts " << response.id;
                        dump(false, ss.str(), a, Poco::Message::PRIO_TRACE);
                        a->response.responseData.push_back(response);
                        accepted = true;
                    }
                    else
                    {
                    }
                }
                if (!accepted)
                {
                    // std::cout << "      NOT ACCEPTED BY ANY OF " << activeRequests.size() << " LISTENERS" << std::endl;
                }
            } catch (std::range_error &e) {
                // this is normal
                //std::cout << "nothing received" << std::endl;
            }

            Poco::Clock now;

            for (auto it = activeRequests.begin(); it != activeRequests.end();) {
                auto a = *it;

                const ResponseInfo &responseInfo = a->request.responseInfo();

                if (responseInfo.isDoneFunction && responseInfo.isDoneFunction(a->response.responseData)) {
                    // std::cout << formatMessageId(a->request.request().requestId) << " SUCCCESS" << std::endl;
                    a->response.status = CanRequestStatus::Success;
                    dump(false, "DONE", a, Poco::Message::PRIO_INFORMATION);
                    a->callback(a->response);
                    it = activeRequests.erase(it);
                } else if (now > a->dueAt) {
                    if (responseInfo.successOnTimeout) {
                        // std::cout << formatMessageId(a->request.request().requestId) << " SUCCCESS (to)" << std::endl;
                        a->response.status = CanRequestStatus::Success;
                        dump(false, "DONE after timeout", a, Poco::Message::PRIO_INFORMATION);
                        a->callback(a->response);
                    } else {
                        // std::cout << formatMessageId(a->request.request().requestId) << " Timeout" << std::endl;
                        dump(false, "TIMEOUT", a, Poco::Message::PRIO_WARNING);
                        a->response.status = CanRequestStatus::Timeout;
                        a->callback(a->response);
                    }
                    it = activeRequests.erase(it);
                } else {
                    it++;
                }
            }
        } catch (std::exception &e) {
            LERROR("CAN") << "Failed to receive CAN message (" << e.what() << ")" << LE;
        } catch (...) {
            LERROR("CAN") << "Failed to receive CAN message (unknown error)" << LE;
        }
    }
}
