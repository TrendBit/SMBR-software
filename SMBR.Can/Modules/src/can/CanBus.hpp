#ifndef CANBUS_HPP
#define CANBUS_HPP

#include <linux/can.h>
#include <linux/can/raw.h>
#include <functional>
#include <vector>
#include <memory>
#include "CanMessage.hpp"

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>

/**
 * @class CanBus
 * @brief Class responsible for CAN bus communication.
 * 
 * CanBus manages sending and receiving CAN messages asynchronously
 * using Boost Asio for asynchronous I/O operations.
 */
class CanBus  {
public:
    /**
     * @brief Constructor for CanBus.
     * 
     * Initializes the CAN bus on the specified interface (can0) and prepares it for communication.
     * 
     */
    CanBus();

    /**
     * @brief Destructor for CanBus.
     * 
     * Closes the CAN socket and cleans up resources.
     */
    virtual ~CanBus();

    /**
     * @brief Asynchronously send a CAN message.
     * 
     * Sends the specified CAN message and calls the handler once the operation is complete.
     * 
     * @param message The CAN message to send.
     */
    bool send(const CanMessage& message);

    /**
     * @brief Asynchronously receive CAN messages.
     * 
     * Continuously listens for incoming CAN messages and calls the handler with each message received.
     * 
     * @param handler Callback function to handle the received messages.
     */
    CanMessage receive(int timeoutMs);

private:    

    int socketFd;
    struct sockaddr_can addr; 
    //struct ifreq ifr;
};

#endif 
