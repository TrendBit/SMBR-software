#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Data Transfer Object representing fluorometer capture status.
 */
class FluorometerCaptureStatusDto : public oatpp::DTO {
    DTO_INIT(FluorometerCaptureStatusDto, DTO)

    /**
     * @brief Indicates if the fluorometer capture is complete.
     */
    DTO_FIELD(Boolean, capture_complete); 
};

#include OATPP_CODEGEN_END(DTO)