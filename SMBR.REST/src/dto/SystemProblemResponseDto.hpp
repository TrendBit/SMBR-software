#pragma once

#include "oatpp/Types.hpp"
#include "oatpp/macro/codegen.hpp"
#include "dto/SystemProblemDto.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class SystemProblemResponseDto : public oatpp::DTO {
  DTO_INIT(SystemProblemResponseDto, DTO)

  DTO_FIELD(String, message);
  DTO_FIELD(List<Object<SystemProblemDto>>, problems);
};

#include OATPP_CODEGEN_END(DTO)