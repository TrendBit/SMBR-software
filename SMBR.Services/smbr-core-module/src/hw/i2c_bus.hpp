/**
 * @file i2c_bus.hpp
 * @author Petr Malaník (TheColonelYoung(at)gmail(dot)com)
 * @version 0.1
 * @date 6.12.2023
 */
#pragma once

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <iostream>
#include <stdexcept>
#include <cstdint>
#include <string>
#include <vector>
#include <optional>

/**
 * @brief   Class for communication over I2C bus
 */
class I2C_bus{
private:
    /**
     * @brief I2C instance file descriptor
     */
    int descriptor;

    /**
     * @brief Baudrate of I2C bus
     */
    const uint32_t speed;

public:
    /**
     * @brief Construct a new I2C_bus object
     *
     * @param device    Path to I2C device
     * @param speed     Speed of I2C bus in Hz
     */
    I2C_bus(const std::string &device, uint32_t speed = 100'000);

    /**
     * @brief Destroy the i2c bus object
     */
    ~I2C_bus();

    /**
     * @brief   Write data to I2C bus
     *
     * @param address   Address of slave device
     * @param data      Data to be written
     * @param length    Length of data to be written
     * @param timeout_us    Timeout for write operation in microseconds
     *                      If not specified, blocking write is used
     * @return int      Number of bytes written, or PICO_ERROR_GENERIC if address not acknowledged or timeout
     */
    int Write(uint8_t address, const uint8_t *data, uint length, uint timeout_us = 0);

    /**
     * @brief   Write data to I2C bus
     *
     * @param address   Address of slave device
     * @param data      Data to be written
     * @param timeout_us    Timeout for write operation in microseconds
     *                      If not specified, blocking write is used
     * @return int      Number of bytes written, or PICO_ERROR_GENERIC if address not acknowledged or timeout
     */
    int Write(uint8_t address, const std::vector<uint8_t> &data, uint timeout_us = 0);

    /**
     * @brief   Read data from I2C bus
     *
     * @param address   Address of slave device
     * @param data      Pointer to buffer where data will be stored
     * @param length    Length of data to be read
     * @param timeout_us    Timeout for read operation in microseconds
     *                      If not specified, blocking read is used
     */
    int Read(uint8_t address, uint8_t *data, uint length, uint timeout_us = 0);

    /**
     * @brief   Read data from I2C bus
     *
     * @param address   Address of slave device
     * @param length    Length of data to be read
     * @param timeout_us    Timeout for read operation in microseconds
     *                      If not specified, blocking read is used
     * @return std::optional<std::vector<uint8_t>>   Vector of read data if valid, false otherwise
     */
    std::optional<std::vector<uint8_t>> Read(uint8_t address, uint length, uint timeout_us = 0);
};
