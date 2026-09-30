#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Whether a manager currently actively controls this reactor, and which one.
 */
class ControlledDto : public oatpp::DTO {
    DTO_INIT(ControlledDto, DTO)

    /**
     * @brief Id of the manager asserting the flag. Empty/absent when controlled is false.
     */
    DTO_FIELD(String, manager_id);

    /**
     * @brief True while a manager actively controls this reactor.
     */
    DTO_FIELD(Boolean, controlled);
};

#include OATPP_CODEGEN_END(DTO)
