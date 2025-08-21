#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Data Transfer Object representing TX errors.
 */
class TxErrorsDto : public oatpp::DTO {
    DTO_INIT(TxErrorsDto, DTO)

    /**
     * @brief Number of errors during sending
     */
    DTO_FIELD(UInt64, tx_errors); 
};

#include OATPP_CODEGEN_END(DTO)
