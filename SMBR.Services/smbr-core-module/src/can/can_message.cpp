#include "can_message.hpp"

#include <algorithm>

CAN::Message::Message(uint32_t id, bool extended, bool remote_request)
    :
    id(id),
    extended(extended),
    remote_request(remote_request),
    data(std::vector<uint8_t>(0)) { }

CAN::Message::Message(uint32_t id, uint8_t length, bool extended, bool remote_request)
    :
    id(id),
    extended(extended),
    remote_request(remote_request),
    data(std::vector<uint8_t>(length, 0)) { }

CAN::Message::Message(uint32_t id, std::vector<uint8_t> data, bool extended, bool remote_request)
    :
    id(id),
    extended(extended),
    remote_request(remote_request),
    data(data) { }

CAN::Message::Message(struct can_frame *frame)
    : id(frame->can_id & ~(CAN_EFF_FLAG | CAN_RTR_FLAG)),
    extended(frame->can_id & CAN_EFF_FLAG),
    remote_request(frame->can_id & CAN_RTR_FLAG),
    data(std::vector<uint8_t>(frame->data, frame->data + frame->can_dlc)) { }

uint32_t CAN::Message::ID() const {
    return id;
}

bool CAN::Message::Extended() const {
    return extended;
}

bool CAN::Message::Remote() const {
    return remote_request;
}

struct can_frame CAN::Message::to_msg() const {
    struct can_frame frame = { };
    frame.can_id = id;

    if (extended) {
        frame.can_id |= CAN_EFF_FLAG;
    }

    if (remote_request) {
        frame.can_id |= CAN_RTR_FLAG;
    }

    frame.can_dlc = data.size();
    std::copy(data.begin(), data.end(), frame.data);

    return frame;
}
