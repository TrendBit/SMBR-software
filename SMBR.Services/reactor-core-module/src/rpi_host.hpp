/**
 * @file rpi_host.hpp
 * @author Petr Malaník (TheColonelYoung(at)gmail(dot)com)
 * @version 0.1
 * @date 18.01.2025
 */

#pragma once

#include "rpi_host.hpp"

#include <fstream>
#include <string>
#include <array>
#include <cstdint>
#include <sstream>
#include <iomanip>
#include <functional>
#include <optional>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <openssl/sha.h>

/**
 * @brief   Class for reading system info from Raspberry Pi
 */
class RPi_host {
public:
    /**
     * @brief Construct a new RPi object
     */
    RPi_host() = default;

    /**
     * @brief   Read core temperature from Raspberry Pi
     *
     * @return float
     */
    float Core_temperature();

    /**
     * @brief   Read core load from Raspberry Pi (all core 1 minute average)
     *          Load is in range 0-1.0 and represents all core at one, (3 cores idle, 1 core at 100% = 0.25)
     *
     * @return float
     */
    float Core_load();

    /**
     * @brief   Get unique 6-byte device ID
     * @return  std::array<uint8_t, 6> Device UID
     */
    std::array<uint8_t, 6> Device_UID();

    /**
     * @brief   Get short 2-byte device ID
     * @return  uint16_t Short device ID
     */
    uint16_t Short_ID();

    /**
     * @brief   Get IP address as byte array
     * @return  std::optional<std::array<uint8_t, 4>> IP address or empty if not found
     */
    std::optional<std::array<uint8_t, 4>> IP_address();

    /**
     * @brief   Get hostname of Raspberry Pi
     *          Maximal supported length is 8 characters, longer are truncated, limited die to CAN payload size
     * @return  std::string Hostname or empty string if error (8 chars max)
     */
    std::string Hostname();

    /**
     * @brief   Get serial number of Raspberry Pi
     * @return  uint32_t Serial number
     */
    uint32_t Serial_number();

private:
    /**
     * @brief   Read serial number from /proc/cpuinfo
     * @return  std::string Serial number or empty string if not found
     */
    std::string Read_serial();

    /**
     * @brief   Hash string into 6-byte array
     * @param   input String to hash
     * @return  std::array<uint8_t, 6> Hash result
     */
    std::array<uint8_t, 6> Hash(const std::string& input);
};
