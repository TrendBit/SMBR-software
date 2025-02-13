/**
 * @file gpio.hpp
 * @author Petr Malaník (TheColonelYoung(at)gmail(dot)com)
 * @version 0.1
 * @date 18.01.2025
 */

#include <stdexcept>
#include <cstdint>

/**
 * @brief   GPIO pin control class for direct memory access
 */
class GPIO {
private:
    /**
     * @brief   GPIO pin number
     */
    int gpio_pin;

    /**
     * @brief   Pointer to mapped GPIO registers
     */
    uint32_t *gpio_base;

    /**
     * @brief   Base address of GPIO peripheral
     * @note    For Raspberry Pi 3/4
     */
    static const uint32_t GPIO_BASE = 0x3F200000;

    /**
     * @brief   Length of GPIO register space
     */
    static const uint32_t GPIO_LEN = 0xB4;

public:
    /**
     * @brief   GPIO direction enumeration
     */
    enum class Direction {
        In,
        Out
    };

    /**
     * @brief   Construct a new GPIO object
     * @param   pin GPIO pin number
     * @param   dir Initial pin direction
     */
    explicit GPIO(int pin, Direction dir = Direction::In);

    /**
     * @brief   Destroy the GPIO object and cleanup resources
     */
    ~GPIO();

    /**
     * @brief   Set GPIO pin direction
     * @param   dir New direction
     */
    void Set_direction(Direction dir);

    /**
     * @brief   Read GPIO pin state
     * @return  bool Current pin state
     */
    bool Read() const;

    /**
     * @brief   Set GPIO pin output value
     * @param   value New pin state
     */
    void Set_value(bool value) const;

private:
    /**
     * @brief   Memory map descriptor
     */
    void *gpio_map;

    /**
     * @brief   File descriptor for /dev/mem
     */
    int mem_fd;

    /**
     * @brief   Initialize GPIO memory mapping
     */
    void Init_GPIO();

    /**
     * @brief   Cleanup GPIO memory mapping
     */
    void Deinit_GPIO();
};
