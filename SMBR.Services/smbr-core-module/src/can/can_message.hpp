/**
 * @file can_message.hpp
 * @author Petr Malaník (TheColonelYoung(at)gmail(dot)com)
 * @version 0.1
 * @date 01.06.2024
 */

#pragma once

#include <linux/can.h>
#include <vector>
#include <cstdint>


namespace CAN {
/**
 * @brief   Representation of CAN message, including ID, data and flags
 *          Support for CAN2.0B extended and remote frames with 29 bit identifiers
 */
class Message {
protected:
    /**
     * @brief Message identifier, 11 or 29 bit (extension frame) are valid
     */
    uint32_t id;

    /**
     * @brief Flag for extended frame, if true, 29 bit identifier is used
     */
    bool extended = false;

    /**
     * @brief Remote request frame flag
     */
    bool remote_request = false;

public:
    /**
     * @brief Data of message, maximum 8 bytes
     */
    std::vector<uint8_t> data;

public:
    /**
     * @brief Construct a new Message object without data
     *
     * @param id            Message identifier
     * @param extended      Flag for extended frame
     * @param remote_request    Flag for remote request frame
     */
    explicit Message(uint32_t id, bool extended = false, bool remote_request = false);

    /**
     * @brief Construct a new Message object, with data of requested size initialized to 0
     *
     * @param id                Message identifier
     * @param length            Length of data, maximum 8 bytes
     * @param extended          Flag for extended frame
     * @param remote_request    Flag for remote request frame
     */
    explicit Message(uint32_t id, uint8_t length, bool extended = false, bool remote_request = false);

    /**
     * @brief Construct a new Message object with supplied data
     *
     * @param id                Message identifier
     * @param data              Data of message (vector), maximum 8 bytes
     * @param extended          Flag for extended frame
     * @param remote_request    Flag for remote request frame
     */
    explicit Message(uint32_t id, std::vector<uint8_t> data, bool extended = false, bool remote_request = false);

    /**
     * @brief Construct a new Message object from CAN frame
     *
     * @param frame  Pointer to CAN frame
     */
    explicit Message(struct can_frame *frame);

    /**
     * @brief Get the ID of CAN bus message
     *
     * @return uint32_t Message identifier, 11 or 29 bit are valid based on extended flag
     */
    uint32_t ID() const;

    /**
     * @brief  Check if message is extended frame
     *
     * @return true     Message is extended - 29 bit identifier
     * @return false    Message is standard - 11 bit identifier
     */
    bool Extended() const;

    /**
     * @brief Check if message is remote request frame
     *
     * @return true    Message is remote request frame
     * @return false   Message is normal frame
     */
    bool Remote() const;

    /**
     * @brief Convert Message object to CAN frame
     *
     * @return can_frame   Linux CAN frame representation of this message
     */
    struct can_frame to_msg() const;
};
}
