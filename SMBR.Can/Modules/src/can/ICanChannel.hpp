#pragma once

#include <memory>
#include <vector>
#include <optional>
#include <future>
#include <ostream>
#include "CanRequest.hpp"

class ICanChannel {
public:

    typedef std::shared_ptr <ICanChannel> Ptr;

    virtual ~ICanChannel() = default;

    struct Response {
        CanRequestStatus status = CanRequestStatus::Unknown;
        CanRequest request;
        std::vector <ResponseData> responseData;

        Response(CanRequest request) : request(request) {}

    };

    virtual void send(const CanRequest & canRequest, std::function <void(Response)>) = 0;
};

std::ostream & operator<<(std::ostream & os, const ICanChannel::Response & r);
