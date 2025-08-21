#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Data Transfer Object representing RX packets.
 */
class RxPacketsDto : public oatpp::DTO {
    DTO_INIT(RxPacketsDto , DTO)

    /**
     * @brief Number of received CAN packets
     */
    DTO_FIELD(UInt64, rx_packets); 
};

#include OATPP_CODEGEN_END(DTO)
