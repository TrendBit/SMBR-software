#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

namespace dto {

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Enumeration representing different channels.
 */
ENUM(ChannelEnum, v_int32,
     VALUE(channel0, 0, "channel0"),
     VALUE(channel1, 1, "channel1"),
     VALUE(channel2, 2, "channel2"),
     VALUE(channel3, 3, "channel3"))

#include OATPP_CODEGEN_END(DTO)

}
