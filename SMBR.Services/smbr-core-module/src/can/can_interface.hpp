/**
 * @file can_interface.hpp
 * @author Petr Malaník (TheColonelYoung(at)gmail(dot)com)
 * @version 0.1
 * @date 18.01.2025
 */

#pragma once

#include <algorithm>
#include <cstring>
#include <chrono>
#include <iostream>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <optional>
#include <stdexcept>
#include <string>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>

#include "codes/codes.hpp"
#include "can/can_message.hpp"
#include "can/app_message.hpp"

namespace CAN {

class Interface {
private:
    int socket_fd;

    const Codes::Module module_type = Codes::Module::Core_module;

    const Codes::Instance instance = Codes::Instance::Exclusive;

    std::vector<Codes::Message_type> accepted_messages = {
        Codes::Message_type::Ping_request,
        Codes::Message_type::Probe_modules_request,
        Codes::Message_type::Core_load_request,
        Codes::Message_type::Core_temperature_request,
        Codes::Message_type::Board_temperature_request,
        Codes::Message_type::Core_SID_request,
        Codes::Message_type::Core_IP_request,
        Codes::Message_type::Core_hostname_request,
        Codes::Message_type::Core_serial_request,
        Codes::Message_type::Supply_type_request,
        Codes::Message_type::Supply_5V_rail_request,
        Codes::Message_type::Supply_VIN_rail_request,
        Codes::Message_type::Supply_POE_rail_request,
        Codes::Message_type::Supply_current_request,
        Codes::Message_type::Supply_power_draw_request,
    };

public:
    Interface(const std::string& interface);

    ~Interface();

    Application_message Receiver_loop();

    /**
     * @brief   Send Application message via CAN bus
     * @param   message Application message to send
     * @return  bool True if successful
     */
    bool Send_message(const Application_message& message);

private:

    std::optional<Codes::Message_type> Process_message(Application_message &app_message);

    /**
     * @brief   Initialize CAN socket
     * @param   interface CAN interface name
     * @return  true if successful
     */
    bool Init_socket(const std::string& interface);
};

}
