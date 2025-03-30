#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Data Transfer Object representing single spectrophotometer channel measurement.
 */
class SingleChannelMeasurementDto : public oatpp::DTO {
    DTO_INIT(SingleChannelMeasurementDto, DTO)
    
    /**
     * @brief Channel number identifier (0 to N-1 where N is total number of channels).
     */
    DTO_FIELD(Int8, channel);

    /**
     * @brief Relative absorbance measurement value.
     */
    DTO_FIELD(Float32, relative_value);
};