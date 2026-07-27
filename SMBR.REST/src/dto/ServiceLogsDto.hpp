#pragma once

#include "oatpp/Types.hpp"
#include "oatpp/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Recent journal entries for a managed service.
 */
class ServiceLogsDto : public oatpp::DTO {
    DTO_INIT(ServiceLogsDto, DTO)

    /**
     * @brief Systemd unit name.
     */
    DTO_FIELD(String, name);

    /**
     * @brief Most recent log lines, exactly as printed by `journalctl`, oldest first.
     */
    DTO_FIELD(List<String>, lines);
};

#include OATPP_CODEGEN_END(DTO)
