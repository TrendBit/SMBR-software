#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Data Transfer Object (DTO) representing fluorometer emitor information.
 */
class FluorometerEmitorInfoDto : public oatpp::DTO {
  
  DTO_INIT(FluorometerEmitorInfoDto, DTO)  

  /**
   * @brief Peak wavelength in nm.
   */
  DTO_FIELD(oatpp::UInt16, peak_wavelength);       

  /**
   * @brief Max power output in mW.
   */
  DTO_FIELD(oatpp::UInt16, power_output);       
};

#include OATPP_CODEGEN_END(DTO)