/**
 * @file TLA2024.hpp
 * @author Petr Malaník (TheColonelYoung(at)gmail(dot)com)
 * @version 0.1
 * @date 02.12.2024
 */

#pragma once

#include <cstdint>
#include <optional>

#include "i2c_device.hpp"

/**
 * @brief   4-channel 12-bit ADC TLA2024 connected over I2C
 */
class TLA2024: public I2C_device {
private:
    /**
     * @brief   Register map of TLA2024
     */
    enum class Register : uint8_t {
        Conversion = 0x00,
        Config     = 0x01,
    };

public:
    /**
     * @brief   Data conversion rate of sensor
     */
    enum class Data_rate : uint8_t {
        DR_128  = 0b000,
        DR_250  = 0b001,
        DR_490  = 0b010,
        DR_920  = 0b011,
        DR_1600 = 0b100,
        DR_2400 = 0b101,
        DR_3300 = 0b110,
    };

    /**
     * @brief   Mode of operation of sensor
     */
    enum class Mode : uint8_t {
        Single_shot = 0b1,
        Continuous  = 0b0,
    };

    /**
     * @brief   Programmable gain amplifier of sensor
     *          Serves as reference voltage for ADC
     *          6_144V means that measured value is referenced against 6.144V
     *          For single ended measurements, reference is double of selected value
     *          Signal connected to ADc input should not be higher then VCC
     */
    enum class PGA : uint8_t {
        FSR_6_144 = 0b000,
        FSR_4_096 = 0b001,
        FSR_2_048 = 0b010,
        FSR_1_024 = 0b011,
        FSR_0_512 = 0b100,
        FSR_0_256 = 0b101,
    };

    /**
     * @brief   Possible configuration of inputs for measurement
     *          In format X_Y where X is positive input and Y is negative input
     *          AINx_GND are single ended measurements
     *          AINx_AINx are differential measurements
     */
    enum class Channels : uint8_t {
        AIN0_AIN1 = 0b000,
        AIN0_AIN3 = 0b001,
        AIN1_AIN3 = 0b010,
        AIN2_AIN3 = 0b011,
        AIN0_GND  = 0b100,
        AIN1_GND  = 0b101,
        AIN2_GND  = 0b110,
        AIN3_GND  = 0b111,
    };

    /**
     * @brief   Value of status register when written to device
     */
    enum class Status_write : uint8_t {
        No_effect               = 0b00,
        Start_single_conversion = 0b01,
    };

    /**
     * @brief   Value of status register when read from device
     */
    enum class Status_read : uint8_t {
        Conversion_in_progress = 0b00,
        Data_ready             = 0b01,
    };

    /**
     * @brief   Structure of configuration register
     */
    struct __attribute__((packed)) __attribute__((__may_alias__)) Config_register {
        uint8_t   Reserved : 5; // Always write 03h
        Data_rate Data_rate_setting : 3;
        Mode      Mode_setting : 1;
        PGA       Programmable_gain : 3;
        Channels  Input_multiplexer : 3;
        uint8_t   Status : 1;
    };

private:
    /**
     * @brief   Default configuration of device
     */
    Config_register custom_default_config = {
        .Reserved = 0x03,
        .Data_rate_setting = Data_rate::DR_128,
        .Mode_setting = Mode::Continuous,
        .Programmable_gain = PGA::FSR_2_048,
        .Input_multiplexer = Channels::AIN0_GND,
        .Status = static_cast<uint8_t>(Status_write::No_effect),
    };

public:
    /**
     * @brief Construct a new TLA2024 object
     *
     * @param bus       I2C bus where device is connected
     * @param address   Address of device on I2C bus
     */
    TLA2024(I2C_bus &bus, uint8_t address);

    /**
     * @brief   Reference voltage of ADC, now fixed ut should be dependent on PGA
     *
     * @return float    Reference voltage in volts
     */
    float Reference_voltage() const {return 2*2.048f;}

    /**
     * @brief   Read voltage of selected channel from conversion register
     *
     * @return std::optional<float> Voltage of channel in volts, if error occurred return empty optional
     */
    std::optional<float> Read_voltage();

    /**
     * @brief   Read raw value of selected channel from conversion register
     *
     * @return std::optional<uint16_t> Raw value of channel, if error occurred return empty optional
     */
    std::optional<uint16_t> Read_raw();

    /**
     * @brief   Set data rate of ADC
     *
     * @param data_rate Data rate of ADC
     * @return true     Data rate was set successfully
     * @return false    Error during setting data rate
     */
    bool Set_data_rate(Data_rate data_rate);

    /**
     * @brief   Select channel for measurement
     *
     * @param channel   Channel to select
     * @return true     Channel was selected successfully
     * @return false    Error during channel selection
     */
    bool Select_channel(Channels channel);

private:
    /**
     * @brief   Read conversion register of device
     *
     * @return std::optional<uint16_t>  Value of conversion register, if error occurred return empty optional
     */
    std::optional<uint16_t> Read_conversion();

    /**
     * @brief   Read register from device
     *
     * @param reg   Register to read
     * @return std::optional<uint16_t>  Value of register, if error occurred return empty optional
     */
    std::optional<uint16_t> Read_register(Register reg);

    /**
     * @brief   Write configuration register to device
     *
     * @param configuration Configuration to write
     * @return true         Configuration was written successfully
     * @return false        Error during write
     */
    bool Write_configuration(Config_register configuration);

    /**
     * @brief   Read configuration register from device
     *
     * @return std::optional<Config_register> Configuration read from device, if error occurred return empty optional
     */
    std::optional<Config_register> Read_configuration();

};
