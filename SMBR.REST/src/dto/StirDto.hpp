#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Data Transfer Object representing mixer stir parameters.
 */
class StirDto : public oatpp::DTO {
    DTO_INIT(StirDto, DTO)

    /**
     * @brief The RPM value for stirring, expected to be between 0 and 10000.
     */
    DTO_FIELD(Float32, rpm);
    DTO_FIELD_INFO(rpm) {
        info->required = true;
    };

    /**
     * @brief The time duration for stirring in seconds, expected to be between 0 and 3600.
     */
    DTO_FIELD(Float32, time);
    DTO_FIELD_INFO(time) {
        info->required = true;
    };
};

#include OATPP_CODEGEN_END(DTO)
