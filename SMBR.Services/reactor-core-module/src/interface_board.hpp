/**
 * @file interface_board.hpp
 * @author Petr Malaník (TheColonelYoung(at)gmail(dot)com)
 * @version 0.1
 * @date 18.01.2025
 */

#pragma once

#include "hw/gpio.hpp"
#include "hw/i2c_bus.hpp"
#include "hw/TLA2024.hpp"
#include "hw/TMP102.hpp"

/**
 * @brief   Represent hardware interface module expanding functionality of Raspberry Pi
 */
class Interface_board {
private:
    /**
     * @brief   ADC for measuring voltages from onboard dividers
     */
    TLA2024* const adc;

    /**
     * @brief   Temperature sensor for board temperature measurement
     */
    TMP102* const temp_sensor;

    /**
     * @brief   GPIO input for PoE power detection
     */
    GPIO* poe_detect;

    /**
     * @brief   GPIO input for PoE power budget status
     */
    GPIO* poe_budget;

    /**
     * @brief   GPIO input for Vin voltage detection
     */
    GPIO* vin_detect;

public:
    /**
     * @brief Construct a new Interface_board object
     *
     * @param bus   I2C bus to which all devices are connected
     */
    Interface_board(I2C_bus* bus);

    /**
     * @brief Destroy the Interface_board object
     */
    ~Interface_board();

    /**
     * @brief   Measures 5V rail voltage
     * @return  float Voltage in volts
     */
    float Voltage_5V();

    /**
     * @brief   Measures input voltage from power jack
     * @return  float Voltage in volts
     */
    float Voltage_Vin();

    /**
     * @brief   Measures PoE voltage
     * @return  float Voltage in volts
     */
    float Voltage_PoE();

    /**
     * @brief   Measures current consumption
     * @return  float Current in amperes
     */
    float Current();

    /**
     * @brief   Calculates power consumption
     * @return  float Power in watts
     */
    float Power_draw();

    /**
     * @brief   Checks PoE power presence
     * @return  bool True if PoE power is present
     */
    bool PoE_status();

    /**
     * @brief   Checks PoE power budget availability
     * @return  bool True if power budget is sufficient
     */
    bool PoE_budget();

    /**
     * @brief   Checks power jack input voltage presence
     * @return  bool True if input voltage is present
     */
    bool Vin_status();

    /**
     * @brief   Measures board temperature
     * @return  float Temperature in degrees Celsius
     */
    float Board_temperature();

private:
    /**
     * @brief   Measures voltage on selected channel of adc
     * @param   channel Channel to measure
     * @return  float Voltage in volts
     */
    float Measure_voltage(TLA2024::Channels channel);
};
