#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Data Transfer Object representing custom text for Mini OLED display.
 */
class MyTextDto : public oatpp::DTO {
    DTO_INIT(MyTextDto, DTO)
    /**
     * @brief The custom text to be displayed on Mini OLED.
     */
    DTO_FIELD(String, text);
};

#include OATPP_CODEGEN_END(DTO)