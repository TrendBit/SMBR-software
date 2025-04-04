#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"
#include "dto/CaptureEnumDto.hpp"
#include "dto/FluorometerSampleDto.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Data Transfer Object (DTO) representing a fluorometer measurement.
 */
class FluorometerMeasurementDto : public oatpp::DTO {

  DTO_INIT(FluorometerMeasurementDto, DTO)  

  /**
   * @brief Unique identifier for the measurement.
   */
  DTO_FIELD(UInt32, measurement_id);  

  /**
   * @brief Flag indicating if the data has been read.
   */
  DTO_FIELD(Boolean, read);  

  /**
   * @brief Flag indicating if the data has been saturated.
   */
  DTO_FIELD(Boolean, saturated);  

  /**
   * @brief Gain setting of the detector.
   */
  DTO_FIELD(Enum<dto::GainEnum>::AsString, detector_gain); 

  /**
   * @brief Intensity of the emitor.
   */
  DTO_FIELD(Float32, emitor_intensity);  

  /**
   * @brief Timing configuration for the measurement.
   */
  DTO_FIELD(Enum<dto::TimingEnum>::AsString, timebase);  

  /**
   * @brief Length of the measurement in milliseconds.
   */
  DTO_FIELD(UInt16, length_ms);  

  /**
   * @brief Number of required samples for the measurement.
   */
  DTO_FIELD(UInt16, required_samples);  

  /**
   * @brief Number of captured samples.
   */
  DTO_FIELD(UInt16, captured_samples);  

  /**
   * @brief Number of missing samples.
   */
  DTO_FIELD(UInt16, missing_samples);  

  /**
   * @brief Vector of fluorometer samples.
   */
  DTO_FIELD(Vector<Object<FluorometerSampleDto>>, samples);
};

#include OATPP_CODEGEN_END(DTO)