#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"
#include "dto/CaptureEnumDto.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Data Transfer Object representing the request to start OJIP capture.
 */
class FluorometerOjipCaptureRequestDto : public oatpp::DTO {
    DTO_INIT(FluorometerOjipCaptureRequestDto, DTO)

    /**
     * @brief Intensity of the emitter.
     */
    DTO_FIELD(Float32, emitor_intensity);

    /**
     * @brief Sample timing.
     */
    DTO_FIELD(Enum<dto::TimingEnum>::AsString, timebase);

    /**
     * @brief Length of the measurement in milliseconds.
     */
    DTO_FIELD(UInt16, length_ms);

    /**
     * @brief Number of samples to be taken.
     */
    DTO_FIELD(UInt16, sample_count);

};

#include OATPP_CODEGEN_END(DTO)