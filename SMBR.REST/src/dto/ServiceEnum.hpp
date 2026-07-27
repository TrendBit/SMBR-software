#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

namespace dto {

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Identifier of a systemd service managed by the SMBR software stack.
 */

ENUM(ServiceEnum, v_int32,
     VALUE(core_module, 0, "core-module"),
     VALUE(api_server, 1, "api-server"),
     VALUE(web_control_ts, 2, "web-control-ts"),
     VALUE(database_export, 3, "database-export"),
     VALUE(startup_updates, 4, "startup-updates"),
     VALUE(can0, 5, "can0"),
     VALUE(avahi_daemon, 6, "avahi-daemon"),
     VALUE(swupdate, 7, "swupdate"),
     VALUE(telegraf, 8, "telegraf"))

#include OATPP_CODEGEN_END(DTO)

}
