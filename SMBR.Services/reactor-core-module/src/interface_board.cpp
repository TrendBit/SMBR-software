#include "interface_board.hpp"
#include <thread>
#include <chrono>

Interface_board::Interface_board(I2C_bus* bus):
    adc(new TLA2024(*bus, 0x48)),
    temp_sensor(new TMP102(*bus, 0x49))
{
    poe_detect = new GPIO(22, GPIO::Direction::In);
    poe_budget = new GPIO(27, GPIO::Direction::In);
    vin_detect = new GPIO(17, GPIO::Direction::In);
}

Interface_board::~Interface_board()
{
    delete adc;
    delete temp_sensor;
    delete poe_detect;
    delete poe_budget;
    delete vin_detect;
}

float Interface_board::Voltage_5V()
{
    float divider = (20.0+10.0)/10.0;
    return Measure_voltage(TLA2024::Channels::AIN0_GND) * divider;
}

float Interface_board::Voltage_Vin()
{
    float divider = (100.0+10.0)/10.0;
    return Measure_voltage(TLA2024::Channels::AIN2_GND) * divider;
}

float Interface_board::Voltage_PoE()
{
    float divider = (100.0+10.0)/10.0;
    return Measure_voltage(TLA2024::Channels::AIN3_GND) * divider;
}

float Interface_board::Current()
{
    float amplification = 50.0;
    float sense_resistor = 0.01;
    float voltage = Measure_voltage(TLA2024::Channels::AIN1_GND);
    float sense_voltage = voltage / amplification;
    float current = sense_voltage / sense_resistor;
    return current;
}

float Interface_board::Power_draw()
{
    float voltage = Voltage_Vin();
    float current = Current();
    return voltage * current;
}

bool Interface_board::PoE_status()
{
    return not poe_detect->Read();
}

bool Interface_board::PoE_budget()
{
    if (not PoE_status()) {
        return false;
    }
    return poe_budget->Read();
}

bool Interface_board::Vin_status()
{
    return not vin_detect->Read();
}

float Interface_board::Board_temperature()
{
    return temp_sensor->Temperature();
}

float Interface_board::Measure_voltage(TLA2024::Channels channel)
{
    adc->Select_channel(channel);
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    float voltage = adc->Read_voltage().value_or(0.0f);
    return voltage;
}
