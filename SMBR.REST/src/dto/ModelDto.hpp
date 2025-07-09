#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Data Transfer Object representing the model of the device.
 */
class ModelDto : public oatpp::DTO {
    DTO_INIT(ModelDto, DTO)

    /**
     * @brief The model of the device (e.g., RPi4B, RPi3B+, RPiZ2W).
     */
    DTO_FIELD(String, model);
};

#include OATPP_CODEGEN_END(DTO)
