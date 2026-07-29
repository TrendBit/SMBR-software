#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Data Transfer Object representing a request to set the hostname of the device.
 */
class HostnameRequestDto : public oatpp::DTO {
    DTO_INIT(HostnameRequestDto, DTO)

    /**
     * @brief The new hostname to set for the device.
     */
    DTO_FIELD(String, hostname);
};

#include OATPP_CODEGEN_END(DTO)
