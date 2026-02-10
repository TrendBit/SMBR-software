#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Data Transfer Object representing pump information.
 */
class PumpInfoDto : public oatpp::DTO {
    DTO_INIT(PumpInfoDto, DTO)
    
    /**
     * @brief Maximum flow rate of the pump (ml/min)
     */
    DTO_FIELD(Float32, max_flowrate);
    
    /**
     * @brief Minimum flow rate of the pump (ml/min)
     */
    DTO_FIELD(Float32, min_flowrate);
};

#include OATPP_CODEGEN_END(DTO)
