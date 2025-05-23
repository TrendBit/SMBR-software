#pragma once

#include "oatpp/Types.hpp"
#include "oatpp/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Data Transfer Object representing load and core count response.
 */
class LoadResponseDto : public oatpp::DTO {
  
  DTO_INIT(LoadResponseDto, DTO)

  /**
   * @brief The current load as a percentage.
   */
  DTO_FIELD(Float32, load); 
  
};

#include OATPP_CODEGEN_END(DTO)


