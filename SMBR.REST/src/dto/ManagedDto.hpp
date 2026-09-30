#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Whether a manager currently sees this reactor on the network, and which one.
 */
class ManagedDto : public oatpp::DTO {
    DTO_INIT(ManagedDto, DTO)

    /**
     * @brief Id of the manager asserting the flag. Empty/absent when managed is false.
     */
    DTO_FIELD(String, manager_id);

    /**
     * @brief True while a manager sees this reactor.
     */
    DTO_FIELD(Boolean, managed);
};

#include OATPP_CODEGEN_END(DTO)
