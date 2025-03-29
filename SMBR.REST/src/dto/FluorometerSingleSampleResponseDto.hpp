#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Data Transfer Object (DTO) representing a single response fluorometer sample.
 */
class FluorometerSingleSampleResponseDto : public oatpp::DTO {
  
  DTO_INIT(FluorometerSingleSampleResponseDto, DTO)     

  /**
   * @brief Raw value from the sensor.
   */
  DTO_FIELD(Int16, raw_value);       

  /**
   * @brief Relative value calculated from the raw value.
   */
  DTO_FIELD(Float32, relative_value);  

  /**
   * @brief Absolute value calculated from the raw value.
   */
  DTO_FIELD(Float32, absolute_value);  
};

#include OATPP_CODEGEN_END(DTO)