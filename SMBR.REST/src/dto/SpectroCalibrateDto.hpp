#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Data Transfer Object for spectrophotometer calibration.
 */
class SpectroCalibrateDto : public oatpp::DTO {
    DTO_INIT(SpectroCalibrateDto, DTO)

    /**
     * @brief Field for calibration.
     */
    DTO_FIELD(String, calibrationMode);

};

#include OATPP_CODEGEN_END(DTO)
