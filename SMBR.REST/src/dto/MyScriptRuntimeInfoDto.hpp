#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Data Transfer Object representing runtime information of a running script.
 */
class MyScriptRuntimeInfoDto : public oatpp::DTO {
    DTO_INIT(MyScriptRuntimeInfoDto, DTO)
    /**
     * @brief name of the script.
     */
    DTO_FIELD(String, name); 
    /**
     * @brief The final message when scripts ends.
     */
    DTO_FIELD(String, finalMessage);
    /**
     * @brief current stack line info of the script.
     */
    DTO_FIELD(List<Int32>::ObjectWrapper, stack); 
    /**
     * @brief last output lines of the script.
     */
    DTO_FIELD(List<String>::ObjectWrapper, output); 
    /**
     * @brief running status of the script.
     */
    DTO_FIELD(Boolean, running);

    /**
     * @brief time when the script started.
     */
    DTO_FIELD(String, started);
};

#include OATPP_CODEGEN_END(DTO)
