#include "gpio.hpp"
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <iostream>

GPIO::GPIO(int pin, Direction dir) : gpio_pin(pin) {
    Init_GPIO();
    Set_direction(dir);
}

GPIO::~GPIO() {
    Deinit_GPIO();
}

void GPIO::Init_GPIO() {
    mem_fd = open("/dev/gpiomem", O_RDWR | O_SYNC);
    if (mem_fd < 0) {
        throw std::runtime_error("Failed to open /dev/gpiomem");
    }

    gpio_map = mmap(NULL, GPIO_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, GPIO_BASE);
    if (gpio_map == MAP_FAILED) {
        throw std::runtime_error("Failed to mmap GPIO memory");
    }

    gpio_base = (uint32_t *)gpio_map;
}

void GPIO::Deinit_GPIO() {
    if (gpio_map != MAP_FAILED) {
        munmap(gpio_map, GPIO_LEN);
    }
    if (mem_fd >= 0) {
        close(mem_fd);
    }
}

void GPIO::Set_direction(Direction dir) {
    uint32_t gpio_reg = gpio_base[gpio_pin / 10];
    uint32_t shift = (gpio_pin % 10) * 3;

    if (dir == Direction::Out) {
        gpio_reg |= (1 << shift);  // Set the pin to output
    } else {
        gpio_reg &= ~(1 << shift); // Set the pin to input
    }

    gpio_base[gpio_pin / 10] = gpio_reg;
}

bool GPIO::Read() const {
    uint32_t gpio_reg = gpio_base[13 + (gpio_pin / 32)];  // GPLEV0 or GPLEV1
    return (gpio_reg & (1 << (gpio_pin % 32))) != 0;
}

void GPIO::Set_value(bool value) const {
    uint32_t reg_offset = (value ? 7 : 10) + (gpio_pin / 32);
    gpio_base[reg_offset] = (1 << (gpio_pin % 32));
}
