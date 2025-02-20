#include "i2c_device.hpp"

I2C_device::I2C_device(I2C_bus &bus, uint8_t address):
    bus(bus),
    address(address)
{
}

int I2C_device::Write(const uint8_t *data, uint length, uint timeout_us){
    return bus.Write(address, data, length, timeout_us);
}

int I2C_device::Write(const std::vector<uint8_t> &data, uint timeout_us){
    return bus.Write(address, data, timeout_us);
}

int I2C_device::Read(uint8_t *data, uint length, uint timeout_us){
    return bus.Read(address, data, length, timeout_us);
}

std::optional<std::vector<uint8_t>> I2C_device::Read(uint length, uint timeout_us){
    return bus.Read(address, length, timeout_us);
}





