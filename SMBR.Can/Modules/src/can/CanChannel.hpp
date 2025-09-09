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
#include "CanRequest.hpp"

class CanChannel : public ICanChannel {
public:
    using ResponseData = ::ResponseData; 

    CanChannel();
    virtual ~CanChannel();

    void send(const std::string & name, const CanRequest & canRequest, std::function <void(Response)>) override;
    void setReceiveHandler(std::function<void(const ResponseData&)> handler);
private:
    
    void runRead();
    void runWrite();
    std::function<void(const ResponseData&)> receiveHandler_;

public:

    struct ActiveRequest {
        std::string name;
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
