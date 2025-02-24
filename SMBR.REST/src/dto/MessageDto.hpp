#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Data Transfer Object representing a message.
 */
class MessageDto : public oatpp::DTO {
    DTO_INIT(MessageDto, DTO)
    /**
     * @brief The message content.
     */
    DTO_FIELD(String, message);
};

#include OATPP_CODEGEN_END(DTO)