#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Data Transfer Object representing dropped TX packets
 */
class TxDroppedDto : public oatpp::DTO {
    DTO_INIT(TxDroppedDto, DTO)

    /**
     * @brief Number of dropped packets during trasmitting
     */
    DTO_FIELD(UInt64, tx_dropped); 
};

#include OATPP_CODEGEN_END(DTO)
