#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Data Transfer Object representing the request to start OJIP capture.
 */
class FluorometerSingleSampleRequestDto : public oatpp::DTO {
    DTO_INIT(FluorometerSingleSampleRequestDto, DTO)

    /**
     * @brief Intensity of the emitter.
     */
    DTO_FIELD(Float32, emitor_intensity);

};

#include OATPP_CODEGEN_END(DTO)