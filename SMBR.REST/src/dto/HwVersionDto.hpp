#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class HwVersionDto : public oatpp::DTO {
    DTO_INIT(HwVersionDto, DTO)

    /**
     * @brief Version string
     */
    DTO_FIELD(String, version);

};

#include OATPP_CODEGEN_END(DTO)