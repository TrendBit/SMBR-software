#pragma once

#include <memory>
#include <vector>
#include <list>
#include <optional>
#include <future>
#include "ICanChannel.hpp"
#include "CanMessage.hpp"
#include <Poco/Clock.h>
#include <Poco/Thread.h>
#include <Poco/Event.h>
#include "CanBus.hpp"

class CanChannel : public ICanChannel {
public:

    CanChannel();
    virtual ~CanChannel();

    void send(const CanRequest & canRequest, std::function <void(Response)>) override;
private:
    
    void runRead();
    void runWrite();

public:

    struct ActiveRequest {
        int uid;
        CanRequest request;
        Response response;
        std::vector <uint32_t> expected;
        std::function <void(Response)> callback;
        Poco::Clock dueAt;
        Poco::Clock startedAt;
        
        ActiveRequest(const CanRequest & request);
    };
private:
    std::vector <std::shared_ptr<ActiveRequest> > toBeSend;
    std::list <std::shared_ptr<ActiveRequest> > activeRequests;
    std::mutex toBeSendMutex;
    std::mutex activeRequestsMutex;
    Poco::Thread bgThreadRead;
    Poco::Thread bgThreadWrite;
    Poco::Event somethingToWrite;
    bool bgFinished = false;

    CanBus bus;

};
