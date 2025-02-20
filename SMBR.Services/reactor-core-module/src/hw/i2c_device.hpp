/**
 * @file i2c_device.cpp
 * @author Petr Malaník (TheColonelYoung(at)gmail(dot)com)
 * @version 0.1
 * @date 6.12.2023
 */

#pragma once

#include "i2c_bus.hpp"

#include <vector>

class I2C_device {
private:

    /**
     * @brief I2C bus on which device is connected
     */
    I2C_bus & bus;

    /**
     * @brief Address of device on I2C bus
     */
    const uint8_t address;

public:
    /**
     * @brief Construct a new i2c device object
     *
     * @param bus       I2C bus on which device is connected
     * @param address   Address of device on I2C bus
     */
    I2C_device(I2C_bus &bus, uint8_t address);

    /**
     * @brief   Get address of device on I2C bus
     *
     * @return short    Address of device (7-bit)
     */
    short Address() const { return address; };

    /**
     * @brief   Get I2C bus on which device is connected
     *
     * @return I2C_bus& Reference to I2C bus
     */
    I2C_bus & Bus() const { return bus; };

protected:

    /**
     * @brief   Write data to I2C bus
     *
     * @param data      Data to be written
     * @param length    Length of data to be written
     * @param timeout_us    Timeout for read operation in microseconds
     *                      If not specified, blocking read is used
     *
     * @return int      Number of bytes written, or PICO_ERROR_GENERIC if address not acknowledged or timeout
     */
    int Write(const uint8_t *data, uint length, uint timeout = 0);

    /**
     * @brief   Write data to I2C bus
     *
     * @param data      Data to be written
     * @param timeout_us    Timeout for read operation in microseconds
     *                      If not specified, blocking read is used
     *
     * @return int      Number of bytes written, or PICO_ERROR_GENERIC if address not acknowledged or timeout
     */
    int Write(const std::vector<uint8_t> &data, uint timeout = 0);

    /**
     * @brief   Read data from I2C bus
     *
     * @param data      Pointer to buffer where data will be stored
     * @param length    Length of data to be read
     * @param timeout_us    Timeout for read operation in microseconds
     *                      If not specified, blocking read is used
     *
     * @return int      Number of read bytes
     */
    int Read(uint8_t *data, uint length, uint timeout_us = 0);

    /**
     * @brief   Read data from I2C bus
     *
     * @param length    Length of data to be read
     * @param timeout_us    Timeout for read operation in microseconds
     *                      If not specified, blocking read is used
     *
     * @return std::optional<std::vector<uint8_t>>   Vector of read data if valid, false otherwise
     */
    std::optional<std::vector<uint8_t> > Read(uint length, uint timeout_us = 0);

    /**
     * @brief   Generalized approach to read value from I2C device register
     *          Most of the devices follow pattern of write operation with address of register and then read operation
     *
     * @tparam t_register_size  Size/type of register to be read, allowed are uint8_t, uint16_t, uint32_t
     * @param register_address  Address of register to be read
     * @param timeout_us        Timeout for read operation in microseconds
     *                          If not specified, blocking read is used
     *
     * @return std::optional<t_register_size>    Value of register if successful, otherwise there was I2C error
     */

};
