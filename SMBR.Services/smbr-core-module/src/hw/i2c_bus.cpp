#include "i2c_bus.hpp"

I2C_bus::I2C_bus(const std::string &device, uint32_t speed)
    : speed(speed) {
    // Open the I2C device
    descriptor = open(device.c_str(), O_RDWR);
    if (descriptor < 0) {
        throw std::runtime_error("Failed to open I2C device: " + device);
    }
}

I2C_bus::~I2C_bus() {
    if (descriptor >= 0) {
        close(descriptor);
        descriptor = -1;
    }
}

int I2C_bus::Write(uint8_t address, const uint8_t *data, uint length, uint timeout_us) {
    if (ioctl(descriptor, I2C_SLAVE, address) < 0) {
        return -1; // Address not acknowledged
    }

    ssize_t result = write(descriptor, data, length);
    if (result < 0) {
        return -1; // Generic error
    }

    if (timeout_us > 0) {
        usleep(timeout_us);
    }

    return static_cast<int>(result); // Number of bytes written
}

int I2C_bus::Write(uint8_t address, const std::vector<uint8_t> &data, uint timeout_us) {
    return Write(address, data.data(), data.size(), timeout_us);
}

int I2C_bus::Read(uint8_t address, uint8_t *data, uint length, uint timeout_us) {
    if (ioctl(descriptor, I2C_SLAVE, address) < 0) {
        return -1; // Address not acknowledged
    }

    ssize_t result = read(descriptor, data, length);
    if (result < 0) {
        return -1; // Generic error
    }

    if (timeout_us > 0) {
        usleep(timeout_us);
    }

    return static_cast<int>(result); // Number of bytes read
}

std::optional<std::vector<uint8_t>> I2C_bus::Read(uint8_t address, uint length, uint timeout_us) {
    std::vector<uint8_t> data(length);

    int result = Read(address, data.data(), length, timeout_us);
    if (result < 0) {
        return std::nullopt; // Read failed
    }

    data.resize(result); // Adjust size to match actual bytes read
    return data;
}
