#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Data Transfer Object representing pump count information.
 */
class PumpCountDto : public oatpp::DTO {
    DTO_INIT(PumpCountDto, DTO)
    
    /**
     * @brief Number of pumps available in the module instance
     */
    DTO_FIELD(UInt8, pump_count);
};

#include OATPP_CODEGEN_END(DTO)
