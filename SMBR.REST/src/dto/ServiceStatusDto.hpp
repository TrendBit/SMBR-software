#pragma once

#include "oatpp/Types.hpp"
#include "oatpp/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Systemd unit status information (equivalent of `systemctl show`/`systemctl status`).
 */
class ServiceStatusDto : public oatpp::DTO {
    DTO_INIT(ServiceStatusDto, DTO)

    /**
     * @brief Systemd unit name.
     */
    DTO_FIELD(String, name);

    /**
     * @brief Whether the unit definition was found by systemd (e.g. "loaded", "not-found").
     */
    DTO_FIELD(String, load_state);

    /**
     * @brief High-level activation state (e.g. "active", "inactive", "failed").
     */
    DTO_FIELD(String, active_state);

    /**
     * @brief Low-level activation state (e.g. "running", "dead", "exited", "failed").
     */
    DTO_FIELD(String, sub_state);

    /**
     * @brief Whether the unit is enabled to start at boot.
     */
    DTO_FIELD(Boolean, enabled);

    /**
     * @brief PID of the unit's main process, 0 if not running.
     */
    DTO_FIELD(Int32, main_pid);

    /**
     * @brief Timestamp of the last transition into the active state, in systemd's native format.
     */
    DTO_FIELD(String, since);
};

#include OATPP_CODEGEN_END(DTO)
