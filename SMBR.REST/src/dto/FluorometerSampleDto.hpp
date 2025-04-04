#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Data Transfer Object (DTO) representing a single fluorometer sample.
 */
class FluorometerSampleDto : public oatpp::DTO {
  
  DTO_INIT(FluorometerSampleDto, DTO)  

  /**
   * @brief Time in milliseconds.
   */
  DTO_FIELD(Float32, time_ms);       

  /**
   * @brief Raw value from the sensor.
   */
  DTO_FIELD(UInt16, raw_value);       

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