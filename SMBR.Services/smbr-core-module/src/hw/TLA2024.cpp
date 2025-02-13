#include "TLA2024.hpp"

TLA2024::TLA2024(I2C_bus &bus, uint8_t address):
    I2C_device(bus, address)
{
    Write_configuration(custom_default_config);
}

std::optional<uint16_t> TLA2024::Read_raw(){
    return Read_conversion();
}

std::optional<float> TLA2024::Read_voltage(){
    auto raw_value = Read_raw();
    if(raw_value.has_value()){
        return (float) raw_value.value() / (1<<12) * Reference_voltage();
    } else {
        return {};
    }
}


bool TLA2024::Select_channel(Channels channel){
    auto config = Read_configuration();
    if(config.has_value()){
        config.value().Input_multiplexer = channel;
        return Write_configuration(config.value());
    } else {
        return false;
    }
}

std::optional<uint16_t> TLA2024::Read_register(Register reg){
    std::vector<uint8_t> data = {static_cast<uint8_t>(reg)};

    I2C_device::Write(data);

    auto data_read = I2C_device::Read(2);
    if(data_read.has_value()){
        return (data_read.value())[0] << 8 | (data_read.value())[1];
    } else {
        return {};
    }
}

bool TLA2024::Write_configuration(Config_register configuration){
    uint16_t config_value = *reinterpret_cast<uint16_t*>(&configuration);

    std::vector<uint8_t> data = {static_cast<uint8_t>(Register::Config), static_cast<uint8_t>(config_value >> 8), static_cast<uint8_t>(config_value & 0xff)};

    return I2C_device::Write(data);
}

std::optional<TLA2024::Config_register> TLA2024::Read_configuration(){
    auto data = Read_register(Register::Config);
    if(data.has_value()){
        return *reinterpret_cast<Config_register*>(&data.value());
    } else {
        return {};
    }
}

std::optional<uint16_t> TLA2024::Read_conversion(){
    auto data = Read_register(Register::Conversion);
    if(data.has_value()){
        uint16_t conversion = data.value() >> 4;
        return conversion;
    } else {
        return {};
    }
}
