#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Data Transfer Object (DTO) representing fluorometer detector information.
 */
class FluorometerDetectorInfoDto : public oatpp::DTO {
  
  DTO_INIT(FluorometerDetectorInfoDto, DTO)  

  /**
   * @brief Detector peak wavelength in nm.
   */
  DTO_FIELD(oatpp::UInt16, peak_wavelength);

  /**
   * @brief Detector sensitivity (unitless).
   */
  DTO_FIELD(oatpp::UInt16, sensitivity);

  /**
   * @brief Sampling rate in kHz.
   */
  DTO_FIELD(oatpp::UInt16, sampling_rate);
};

#include OATPP_CODEGEN_END(DTO)