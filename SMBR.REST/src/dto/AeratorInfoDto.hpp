#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class AeratorInfoDto : public oatpp::DTO {
    DTO_INIT(AeratorInfoDto, DTO)

    /**
     * @brief Maximum flowrate of the aerator at nominal conditions.
     */
    DTO_FIELD(Float32, max_flowrate);

    /**
     * @brief Minimum flowrate of the aerator at nominal conditions.
     */
    DTO_FIELD(Float32, min_flowrate);
};

#include OATPP_CODEGEN_END(DTO)
