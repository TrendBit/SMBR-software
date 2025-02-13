/**
 * @file core_module.hpp
 * @author Petr Malaník (TheColonelYoung(at)gmail(dot)com)
 * @brief
 * @version 0.1
 * @date 06.12.2024
 */

#pragma once

#include "interface_board.hpp"
#include "rpi_host.hpp"
#include "can/can_interface.hpp"
#include "can/app_message.hpp"

#include "codes/messages/common/ping_request.hpp"
#include "codes/messages/common/ping_response.hpp"
#include "codes/messages/common/probe_modules_response.hpp"
#include "codes/messages/common/core_load_response.hpp"
#include "codes/messages/common/core_temp_response.hpp"
#include "codes/messages/common/board_temp_response.hpp"
#include "codes/messages/core/sid_response.hpp"
#include "codes/messages/core/ip_response.hpp"
#include "codes/messages/core/hostname_response.hpp"
#include "codes/messages/core/serial_response.hpp"
#include "codes/messages/core/supply_type_response.hpp"
#include "codes/messages/core/supply_5v_rail_response.hpp"
#include "codes/messages/core/supply_vin_rail_response.hpp"
#include "codes/messages/core/supply_poe_rail_response.hpp"
#include "codes/messages/core/supply_current_response.hpp"
#include "codes/messages/core/supply_power_draw_response.hpp"

/**
 * @brief   Core module class representing main application logic
 */
class Core_module {
private:

    /**
     * @brief  CAN interface for sending and receiving messages from CAN BUS
     */
    CAN::Interface *const can_interface;

    /**
     * @brief   Interface board hardware abstraction
     */
    Interface_board *const interface;

    /**
     * @brief   Raspberry Pi host controller
     */
    RPi_host *const rpi;

public:

    /**
     * @brief   Construct a new Core module object
     * @param   bus I2C bus instance for hardware communication
     */
    explicit Core_module(I2C_bus *bus);

    /**
     * @brief   Destroy the Core module object
     */
    ~Core_module();

    /**
     * @brief   Test function for hardware verification, will try to execute all data suppliers and print their values
     */
    void Probe() const;

    /**
     * @brief   Main loop of core module, which is waiting for messages from CAN BUS and process them
     */
    void Run();
};
