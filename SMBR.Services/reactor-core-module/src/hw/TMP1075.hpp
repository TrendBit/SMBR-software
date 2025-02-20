/**
 * @file TMP1075.hpp
 * @author Petr Malaník (TheColonelYoung(at)gmail(dot)com)
 * @version 0.1
 * @date 02.12.2024
 */

#pragma once

#include "i2c_device.hpp"

#include <vector>

/**
 * @brief  Simple I2C temperature sensor TMP1075
 *         Only minimal functionality is implemented
 */
class TMP1075: public I2C_device{
private:
    /**
     * @brief Address of temperature register
     */
    uint8_t temp_reg = 0x00;

public:
    /**
     * @brief Construct a new TMP1075 object
     *
     * @param bus       I2C bus to which sensor is connected
     * @param address   Address of sensor on bus
     */
    TMP1075(I2C_bus &bus, unsigned char address);

    /**
     * @brief Get temperature from sensor
     *
     * @return double   Temperature in Celsius
     */
    double Temperature();
};
