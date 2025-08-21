#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Data Transfer Object representing TX packets.
 */
class TxPacketsDto : public oatpp::DTO {
    DTO_INIT(TxPacketsDto, DTO)

    /**
     * @brief Number of transmitted CAN packets
     */
    DTO_FIELD(UInt64, tx_packets); 
};

#include OATPP_CODEGEN_END(DTO)
