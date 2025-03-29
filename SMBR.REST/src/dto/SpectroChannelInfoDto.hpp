#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Data Transfer Object representing spectrophotometer channel information.
 */
class SpectroChannelInfoDto : public oatpp::DTO {
    DTO_INIT(SpectroChannelInfoDto, DTO)

    /**
     * @brief Channel number (0 to N-1).
     */
    DTO_FIELD(oatpp::UInt8, channel);
    
    /**
     * @brief Peak wavelength in nm.
     */
    DTO_FIELD(oatpp::UInt16, peak_wavelength);
    
    /**
     * @brief Half intensity peak width in nm.
     */
    DTO_FIELD(oatpp::UInt8, half_intensity_peak_width);
};

#include OATPP_CODEGEN_END(DTO)