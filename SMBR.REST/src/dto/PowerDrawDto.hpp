#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Data Transfer Object representing the power draw of the device.
 */
class PowerDrawDto : public oatpp::DTO {
    DTO_INIT(PowerDrawDto, DTO)

    /**
     * @brief The power draw of the device.
     */
    DTO_FIELD(Float32, power_draw);
};

#include OATPP_CODEGEN_END(DTO)
