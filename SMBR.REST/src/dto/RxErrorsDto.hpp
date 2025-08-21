#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Data Transfer Object representing RX errors.
 */
class RxErrorsDto : public oatpp::DTO {
    DTO_INIT(RxErrorsDto, DTO)

    /**
     * @brief Number of errors during reception
     */
    DTO_FIELD(UInt64, rx_errors); 
};

#include OATPP_CODEGEN_END(DTO)
