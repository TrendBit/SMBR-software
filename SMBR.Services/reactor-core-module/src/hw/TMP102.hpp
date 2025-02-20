/**
 * @file TMP102.hpp
 * @author Petr Malaník (TheColonelYoung(at)gmail(dot)com)
 * @version 0.1
 * @date 05.12.2024
 */

#pragma once

#include "TMP1075.hpp"

/**
 * @brief   Simple I2C temperature sensor TMP102 derived from TMP1075
 *          Only minimal functionality is implemented
 */
class TMP102: public TMP1075{

public:
    /**
     * @brief Derived from TMP1075
     */
    using TMP1075::TMP1075;
};
