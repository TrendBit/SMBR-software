#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Data Transfer Object representing custom text for Mini OLED display.
 */
class TextDto : public oatpp::DTO {
    DTO_INIT(TextDto, DTO)
    /**
     * @brief The custom text to be displayed on Mini OLED.
     */
    DTO_FIELD(String, text);
};

#include OATPP_CODEGEN_END(DTO)