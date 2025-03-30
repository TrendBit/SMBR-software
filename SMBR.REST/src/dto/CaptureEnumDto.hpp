#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

namespace dto {

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Enumeration representing different detector gain values.
 */
ENUM(GainEnum, v_int32,
     //VALUE(Undefined, 0, "Undefined"),
     VALUE(x1, 1, "x1"),
     VALUE(x10, 2, "x10"),
     VALUE(x50, 3, "x50"),
     VALUE(Auto, 4, "Auto")
     )

/**
 * @brief Enumeration representing different timing bases.
 */
ENUM(TimingEnum, v_int32,
     //VALUE(Undefined, 0, "Undefined"),
     VALUE(Logarithmic, 2, "Logarithmic"),
     VALUE(Linear, 1, "Linear"))

#include OATPP_CODEGEN_END(DTO)

}
