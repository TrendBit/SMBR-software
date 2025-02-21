#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Data Transfer Object representing runtime information of a running script.
 */
class ScriptRuntimeInfoDto : public oatpp::DTO {
    DTO_INIT(ScriptRuntimeInfoDto, DTO)
    /**
     * @brief process id of the running script.
     */
    DTO_FIELD(UInt64, processId); 
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
    DTO_FIELD(Vector<Int32>::ObjectWrapper, stack); 
    /**
     * @brief last output lines of the script.
     */
    DTO_FIELD(Vector<String>::ObjectWrapper, output); 
    /**
     * @brief whether the script has started.
     */
    DTO_FIELD(Boolean, started);
    /**
     * @brief whether the script has stopped.
     */
    DTO_FIELD(Boolean, stopped);
    /**
     * @brief time when the script started.
     */
    DTO_FIELD(String, startedAt);
};

#include OATPP_CODEGEN_END(DTO)
