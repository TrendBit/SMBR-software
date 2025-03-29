#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Data Transfer Object representing spectrophotometer channels count.
 */
class SpectroChannelsDto : public oatpp::DTO {
    DTO_INIT(SpectroChannelsDto, DTO)

    /**
     * @brief The number of available channels (0 to N-1).
     */
    DTO_FIELD(oatpp::UInt8, channels);
};

#include OATPP_CODEGEN_END(DTO)