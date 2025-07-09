#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class CuvettePumpInfoDto : public oatpp::DTO {
    DTO_INIT(CuvettePumpInfoDto, DTO)

    /**
     * @brief Maximum flowrate of the cuvette pump at nominal conditions
     */
    DTO_FIELD(Float32, max_flowrate);

    /**
     * @brief Maximum flowrate of the cuvette pump at nominal conditions
     */
    DTO_FIELD(Float32, min_flowrate);
};

#include OATPP_CODEGEN_END(DTO)
