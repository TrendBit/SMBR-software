#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Data Transfer Object representing the number of RX packet collisions.
 */
class CollisionsDto : public oatpp::DTO {
    DTO_INIT(CollisionsDto, DTO)

    /**
     * @brief Number of RX packet collisions.
     */
    DTO_FIELD(UInt64, collisions); 
};

#include OATPP_CODEGEN_END(DTO)
