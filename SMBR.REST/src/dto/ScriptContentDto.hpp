#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Data Transfer Object representing the content of a script to be uploaded.
 */
class ScriptContentDto : public oatpp::DTO {
    DTO_INIT(ScriptContentDto, DTO)
    /**
     * @brief The content of the script to be uploaded.
     */
    DTO_FIELD(String, content); 
};

#include OATPP_CODEGEN_END(DTO)
