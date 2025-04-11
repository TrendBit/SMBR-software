#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Data Transfer Object representing null temperature data.
 */
class TempNullDto : public oatpp::DTO {
    DTO_INIT(TempNullDto, DTO)

    /**
     * @brief The temperature value is null.
     */
    DTO_FIELD(Any, temperature);
};

#include OATPP_CODEGEN_END(DTO)

