#include "CanBus.hpp"
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <functional>
#include <memory>
//include IFNAMSIZ def
#include <net/if.h>
#include "BaseModule.hpp"

CanBus::CanBus(){

    socketFd = ::socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (socketFd < 0) {
        perror("Socket");
        throw std::runtime_error("Failed to create CAN socket");
    }

    const char * iface = "can0";
    struct ifreq ifr;
    std::strncpy(ifr.ifr_name, iface, IFNAMSIZ);

    
    if (ioctl(socketFd, SIOCGIFINDEX, &ifr) < 0) {
        perror("ioctl");
        close(socketFd);
        throw std::runtime_error("Failed to retrieve CAN interface index");
    }

    if (ifr.ifr_ifindex == 0) {
        std::cerr << "Invalid interface index" << std::endl;
        close(socketFd);
        throw std::runtime_error("Invalid interface index");
    }

    std::memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(socketFd, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) < 0) {
        perror("Bind");
        close(socketFd);
        throw std::runtime_error("Failed to bind CAN socket");
    }

}

CanBus::~CanBus() {
    if (socketFd >= 0) {
        close(socketFd);
        std::cout << "CanBus socket closed" << std::endl;
    }
}

bool CanBus::send(const CanMessage& message) {
    //std::cout << "SEND: " << formatMessageId(message.getId()) << std::endl;
    auto frame = std::make_unique<struct can_frame>();
    frame->can_id = message.getId() | CAN_EFF_FLAG;
    frame->can_dlc = message.getData().size();
    std::memcpy(frame->data, message.getData().data(), frame->can_dlc);

    int written = ::write(socketFd, frame.get(), sizeof(*frame));


    if (written != sizeof(*frame)) {
        std::cerr << "Failed to send CAN message with ID: 0x" << std::hex << (frame->can_id & CAN_EFF_MASK) << " and data: ";
        for (size_t i = 0; i < frame->can_dlc; ++i) {
            std::cerr << std::hex << static_cast<int>(frame->data[i]) << " ";
        }
        std::cerr << std::endl;
        return false;
    }
    return true;
}

CanMessage CanBus::receive(int timeoutMs) {
    //wait up to timout, until socket is readable
    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(socketFd, &readSet);

    struct timeval tv;
    tv.tv_sec = timeoutMs / 1000;
    tv.tv_usec = (timeoutMs % 1000) * 1000;

    int ret = select(socketFd + 1, &readSet, nullptr, nullptr, &tv);
    if (ret < 0) {
        perror("Select");
        throw std::runtime_error("Failed to wait for CAN message");
    }

    if (ret == 0) {
        throw std::range_error("Timeout waiting for CAN message");
    }

    auto frame = std::make_unique<struct can_frame>();

    int bytes = ::read(socketFd, frame.get(), sizeof(*frame));

    

    if (bytes != sizeof(*frame)) {
        //std::cout << "RECV: err " << bytes << " sizeof = " << sizeof(*frame) << std::endl;
        throw std::runtime_error("Failed to receive CAN message");
    } else {
        //std::cout << "RECV: " << formatMessageId(frame->can_id) << std::endl;
    }

    std::vector<uint8_t> data(frame->data, frame->data + frame->can_dlc);
    return CanMessage(frame->can_id & CAN_EFF_MASK, data);
}
