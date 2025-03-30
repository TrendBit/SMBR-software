#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"
#include "SingleChannelMeasurementDto.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Data Transfer Object containing all spectrophotometer channel measurements.
 */
class SpectroMeasurementsDto : public oatpp::DTO {
    DTO_INIT(SpectroMeasurementsDto, DTO)
    
    /**
     * @brief Array of measurements for all available channels.
     */
    DTO_FIELD(Vector<Object<SingleChannelMeasurementDto>>, measurements);
};

#include OATPP_CODEGEN_END(DTO)