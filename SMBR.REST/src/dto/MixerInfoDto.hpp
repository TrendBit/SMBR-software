#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class MixerInfoDto : public oatpp::DTO {
    DTO_INIT(MixerInfoDto, DTO)

    /**
     * @brief Maximum reliable RPM of the mixer at normal conditions.
     */
    DTO_FIELD(Int16, max_rpm);

    /**
     * @brief Minimum reliable RPM of the mixer at normal conditions.
     */
    DTO_FIELD(Int16, min_rpm);
};

#include OATPP_CODEGEN_END(DTO)
