#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Data Transfer Object representing dropped RX packets
 */
class RxDroppedDto : public oatpp::DTO {
    DTO_INIT(RxDroppedDto, DTO)

    /**
     * @brief Number of dropped packets during reception
     */
    DTO_FIELD(UInt64, rx_dropped); 
};

#include OATPP_CODEGEN_END(DTO)
