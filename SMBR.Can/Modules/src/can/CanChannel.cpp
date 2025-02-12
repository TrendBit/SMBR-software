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

int nextUID() {
    static int uid = 0;
    return uid++;
}

CanChannel::ActiveRequest::ActiveRequest(const CanRequest &request) : uid(nextUID()), request(request), response(request) {
}

CanChannel::CanChannel() :bgThread("can.bg") {
    bgThread.startFunc([this]() {
        run();
    }); 
}

CanChannel::~CanChannel() {
    bgFinished = true;
    bgThread.join();
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
    // newRequest->dueAt = std::chrono::system_clock::now() + std::chrono::milliseconds(canRequest.responseInfo().timeoutMs);
}

static void dump(std::string message, std::shared_ptr<CanChannel::ActiveRequest> r, Poco::Message::Priority priority = Poco::Message::PRIO_INFORMATION) {
    std::string color = "";
    if (priority == Poco::Message::PRIO_TRACE) {
        //light gray
        color = "\033[38;5;247m";
    }
    if (priority == Poco::Message::PRIO_DEBUG) {
        //light blue
        color = "\033[38;5;147m";
    }
    if (priority == Poco::Message::PRIO_INFORMATION) {
        //light green
        color = "\033[38;5;34m";
    }
    if (priority == Poco::Message::PRIO_NOTICE) {
        //light blue
        color = "\033[38;5;74m";
    }
    if (priority == Poco::Message::PRIO_WARNING) {
        //light yellow
        color = "\033[38;5;148m";
    }
    if (priority == Poco::Message::PRIO_ERROR) {
        //light red
        color = "\033[38;5;196m";
    }
    if (priority <= Poco::Message::PRIO_CRITICAL) {
        //light red
        color = "\033[38;5;200m";
    }
    std::string colorEnd = "\033[39m";

    Poco::LocalDateTime dt;
    std::cout << color << Poco::DateTimeFormatter::format(dt, Poco::DateTimeFormat::SORTABLE_FORMAT)
              << " " << Poco::NumberFormatter::format0(r->uid, 5)
              << " " << r->request.request().id
              << " " << r->response.status
              << " " << message
              << " (after " << r->startedAt.elapsed() / 1000 << "ms)" << colorEnd << std::endl;
}

void CanChannel::run() {

    CanBus bus;

    auto sendRequest = [&](std::shared_ptr<ActiveRequest> request)
    {
        try {
            CanMessage msg(request->request.request().id.id, request->request.request().data);
            bus.send(msg);
            dump("SENT", request, Poco::Message::PRIO_DEBUG);

            {
                std::scoped_lock lock(activeRequestsMutex);
                activeRequests.push_back(request);
            }
        } catch (...) {
            dump("FAILED", request, Poco::Message::PRIO_ERROR);
            request->response.status = CanRequestStatus::Fail;
            request->callback(request->response);
        }
    };

    while (!bgFinished) {
        try {
            std::vector<std::shared_ptr<ActiveRequest>> toSend;
            {
                std::scoped_lock lock(toBeSendMutex);
                toSend = toBeSend;
                toBeSend.clear();
            }

            for (auto s : toSend) {
                sendRequest(s);
            }

            try {
                auto rawResponse = bus.receive(100);
                ResponseData response(rawResponse.getId(), rawResponse.getData());

                // std::cout << "   RECV: " << response.id << std::endl;

                

                bool accepted = false;
                std::scoped_lock lock(activeRequestsMutex);
                for (auto a : activeRequests) {
                    // response info
                    const ResponseInfo &responseInfo = a->request.responseInfo();

                    if (responseInfo.acceptFunction && responseInfo.acceptFunction(response)) {
                        std::stringstream ss;
                        ss << "accepts " << response.id;
                        dump(ss.str(), a, Poco::Message::PRIO_TRACE);
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
                    dump("DONE", a, Poco::Message::PRIO_INFORMATION);
                    a->callback(a->response);
                    it = activeRequests.erase(it);
                } else if (now > a->dueAt) {
                    if (responseInfo.successOnTimeout) {
                        // std::cout << formatMessageId(a->request.request().requestId) << " SUCCCESS (to)" << std::endl;
                        a->response.status = CanRequestStatus::Success;
                        dump("DONE after timeout", a, Poco::Message::PRIO_INFORMATION);
                        a->callback(a->response);
                    } else {
                        // std::cout << formatMessageId(a->request.request().requestId) << " Timeout" << std::endl;
                        dump("TIMEOUT", a, Poco::Message::PRIO_WARNING);
                        a->response.status = CanRequestStatus::Timeout;
                        a->callback(a->response);
                    }
                    it = activeRequests.erase(it);
                } else {
                    it++;
                }
            }
        } catch (std::exception &e) {
            std::cerr << "Failed to receive CAN message (" << e.what() << ")" << std::endl;
        } catch (...) {
            std::cerr << "Failed to receive CAN message (TODO error)" << std::endl;
        }
    }
}
