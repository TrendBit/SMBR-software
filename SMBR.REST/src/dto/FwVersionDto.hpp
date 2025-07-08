#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class FwVersionDto : public oatpp::DTO {
    DTO_INIT(FwVersionDto, DTO)

    /**
     * @brief Version string
     */
    DTO_FIELD(String, version);


    /**
     * @brief Git commit hash of the firmware build.
     */
    DTO_FIELD(String, hash);


    /**
     * @brief Indicates whether the firmware was built from a dirty Git repository.
     */
    DTO_FIELD(Boolean, dirty);
};

#include OATPP_CODEGEN_END(DTO)
