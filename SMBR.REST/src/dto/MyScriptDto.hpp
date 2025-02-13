#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Data Transfer Object representing the content of a script to be uploaded.
 */
class MyScriptDto : public oatpp::DTO {
    DTO_INIT(MyScriptDto, DTO)
    /**
     * @brief The name of the script to be uploaded.
     */
    DTO_FIELD(String, name); 
    /**
     * @brief The content of the script to be uploaded.
     */
    DTO_FIELD(String, content); 
};

#include OATPP_CODEGEN_END(DTO)
