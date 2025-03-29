#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"
#include "dto/CaptureEnumDto.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Data Transfer Object representing request for single fluorometer sample
 */
class FluorometerSingleSampleRequestDto : public oatpp::DTO {
    DTO_INIT(FluorometerSingleSampleRequestDto, DTO)

    /**
     * @brief Emitter intensity (0.2 - 1.0)
     */
    DTO_FIELD(Float32, emitor_intensity);
};

#include OATPP_CODEGEN_END(DTO)