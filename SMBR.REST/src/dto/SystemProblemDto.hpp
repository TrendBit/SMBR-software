#pragma once

#include "oatpp/Types.hpp"
#include "oatpp/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Data Transfer Object containing error information.
 */
class SystemProblemDto : public oatpp::DTO {
  
  DTO_INIT(SystemProblemDto, DTO)

  /**
   * @brief The type of the error.
   */
  DTO_FIELD(String, type);   

  /**
   * @brief Unique error ID.
   */ 
  DTO_FIELD(Int32, id);     

  /**
   * @brief Brief report.
   */
  DTO_FIELD(String, message); 

  /**
   * @brief More detailed description.
   */
  DTO_FIELD(String, detail);  
  
};

#include OATPP_CODEGEN_END(DTO)

