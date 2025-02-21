#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Data Transfer Object representing runtime information of a running script.
 */
class ScriptProcessIdDto : public oatpp::DTO {
    DTO_INIT(ScriptProcessIdDto, DTO)
    /**
     * @brief name of the script.
     */
    DTO_FIELD(UInt64, processId); 
};

#include OATPP_CODEGEN_END(DTO)
