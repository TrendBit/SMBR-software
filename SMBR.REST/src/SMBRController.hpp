#pragma once

#include "oatpp/macro/component.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "dto/ModuleEnum.hpp"
#include "dto/ChannelEnum.hpp"
#include "dto/PingResponseDto.hpp"
#include "dto/TempDto.hpp"
#include "dto/FwVersionDto.hpp"
#include "dto/HwVersionDto.hpp"
#include "dto/TempNullDto.hpp"
#include "dto/ModuleInfoDto.hpp"
#include "dto/LoadResponseDto.hpp"
#include "dto/ModuleActionRequestDto.hpp"
#include "dto/IntensitiesDto.hpp"
#include "dto/IntensityDto.hpp"
#include "dto/SpeedDto.hpp"
#include "dto/FlowrateDto.hpp"
#include "dto/CuvettePumpInfoDto.hpp"
#include "dto/MoveDto.hpp"
#include "dto/AeratorInfoDto.hpp"
#include "dto/RpmDto.hpp"
#include "dto/MixerInfoDto.hpp"
#include "dto/StirDto.hpp"
#include "dto/SIDDto.hpp"
#include "dto/IpDto.hpp"
#include "dto/HostnameDto.hpp"
#include "dto/SerialDto.hpp"
#include "dto/ModelDto.hpp"
#include "dto/SupplyTypeDto.hpp"
#include "dto/VoltageDto.hpp"
#include "dto/CurrentDto.hpp"
#include "dto/PowerDrawDto.hpp"
#include "dto/ScriptDto.hpp"
#include "dto/ScriptProcessIdDto.hpp"
#include "dto/ScriptRuntimeInfoDto.hpp"
#include "dto/TextDto.hpp"
#include "dto/MessageDto.hpp"
#include "dto/FluorometerSingleSampleRequestDto.hpp"
#include "dto/FluorometerSingleSampleResponseDto.hpp"
#include "dto/FluorometerCaptureStatusDto.hpp"
#include "dto/FluorometerOjipCaptureRequestDto.hpp"
#include "dto/CaptureEnumDto.hpp"
#include "dto/FluorometerMeasurementDto.hpp"
#include "dto/FluorometerSampleDto.hpp"
#include "dto/FluorometerDetectorInfoDto.hpp"
#include "dto/FluorometerEmitorInfoDto.hpp"
#include "dto/SpectroChannelsDto.hpp"
#include "dto/SpectroChannelInfoDto.hpp"
#include "dto/SpectroMeasurementsDto.hpp"
#include "dto/SingleChannelMeasurementDto.hpp"
#include "dto/SpectroCalibrateDto.hpp"
#include "dto/SystemProblemDto.hpp"
#include "dto/SystemProblemResponseDto.hpp"
#include "dto/RxPacketsDto.hpp"
#include "dto/TxPacketsDto.hpp"
#include "dto/RxErrorsDto.hpp"
#include "dto/TxErrorsDto.hpp"
#include "dto/RxDroppedDto.hpp"
#include "dto/TxDroppedDto.hpp"
#include "dto/CollisionsDto.hpp"
#include "dto/ModuleIssueDto.hpp"
#include "dto/ModuleIssuesListDto.hpp"
#include "oatpp/data/mapping/ObjectMapper.hpp"

#include <future>
#include <iomanip>
#include <cstdint>
#include <utility>
#include <unordered_set>
#include <optional>
#include <queue>

#include <thread> 
#include <chrono> 
#include <cmath>
#include <iostream>
#include <fstream>

#include "SMBR/ISystemModule.hpp"
#include "SMBR/ISensorModule.hpp"
#include "SMBR/IScheduler.hpp"
#include "SMBR/IRecipes.hpp"


#include OATPP_CODEGEN_BEGIN(ApiController)

//hack to have other methods in allowed list
#undef OATPP_MACRO_API_CONTROLLER_ADD_CORS_BODY_DEFAULT_METHODS
#define OATPP_MACRO_API_CONTROLLER_ADD_CORS_BODY_DEFAULT_METHODS "GET, POST, OPTIONS, PUT, PATCH, DELETE"


/**
 * @class SMBRController
 * @brief Defines API endpoints for handling system, core, and sensor module actions.
 */
class SMBRController : public oatpp::web::server::api::ApiController {
public:
    SMBRController(const std::shared_ptr<oatpp::web::mime::ContentMappers>& apiContentMappers,
                 std::shared_ptr<ISystemModule> systemModule);

public:
// ==========================================
// System Endpoints
// ==========================================

    /**
     * @brief Retrieves available modules and their unique CAN IDs.
     */
    ENDPOINT_INFO(getSystemModules) {
        info->summary = "Retrieves available modules and their respective unique CAN IDs";
        info->addTag("System");
        info->description = "Returns a list of all modules that have responded to the identification message.";
        auto example = List<Object<ModuleInfoDto>>::createShared();
        auto module = ModuleInfoDto::createShared();
        module->module_type = "sensor";
        module->uid = "0x0123456789ab";
        module->instance = "Exclusive"; 
        example->push_back(module);
        info->addResponse<List<Object<ModuleInfoDto>>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getSystemModules)
    ENDPOINT("GET", "/system/modules", getSystemModules);

    /**
     * @brief Lists all detected system errors or confirms that the system is operating normally.
     */
    ENDPOINT_INFO(getSystemErrors) {
    info->summary = "Lists all detected system errors or confirms normal system operation";
    info->addTag("System");
    info->description =
        "Returns all detected system errors.\n\n"
        "**Error types:**\n"
        "  - Multiple instances of the same numbered module instance (e.g., 2x Instance_1) or multiple exclusive modules (e.g., 2x Exclusive)\n"
        "  - Unknown module instance (All, Undefined, or Reserved)\n"
        //"  - Firmware version mismatch between modules of the same type (e.g., multiple pump modules with different firmware versions)\n"
        "  - Unavailability of one of the three modules (core, sensor, control)";
    auto exampleOk = SystemProblemResponseDto::createShared();
    exampleOk->message = "System is operating normally. No errors detected.";
    exampleOk->problems = {};
    auto exampleErrors = SystemProblemResponseDto::createShared();
    exampleErrors->message = "System errors detected. See details in 'problems'.";
    auto e1 = SystemProblemDto::createShared();
    e1->type = "ModuleUnavailable";
    e1->id = 1;
    e1->message = "One or more modules are unavailable";
    e1->detail = "Module sensor not responding";
    auto e2 = SystemProblemDto::createShared();
    e2->type = "UnknownInstance";
    e2->id = 2;
    e2->message = "Unknown module instance detected: Reserved";
    e2->detail = "Module control reported instance value Reserved";
    auto e3 = SystemProblemDto::createShared();
    e3->type = "DuplicateInstance";
    e3->id = 3;
    e3->message = "Multiple instances of the same module instance detected: Instance_1";
    e3->detail = "Detected more than one Instance_1 in module type sensor";
    exampleErrors->problems = { e1, e2, e3 };

    info->addResponse<Object<SystemProblemResponseDto>>(Status::CODE_200, "application/json")
        .addExample("No Errors", exampleOk)
        .addExample("Errors", exampleErrors);
    }
    ADD_CORS(getSystemErrors)
    ENDPOINT("GET", "/system/errors", getSystemErrors);

    /**
    * @brief Lists all detected system warnings or confirms that the system is operating normally.
    */
    ENDPOINT_INFO(getSystemWarnings) {
        info->summary = "Lists all detected system warnings or confirms normal system operation";
        info->addTag("System");
        info->description =
            "Returns all detected system warnings.\n\n"
            "**Warning types:**\n"
            "  - Firmware version mismatch between modules of different types (excluding core modules)\n"
            "  - Dirty build firmware detected on a module\n"
            "  - CAN bus unreachable or CAN error rate above threshold\n"
            "  - High ping time (>500 ms) detected for one of the three modules (core, sensor, control)";
        auto exampleOk = SystemProblemResponseDto::createShared();
        exampleOk->message = "System is operating normally. No warnings detected.";
        exampleOk->problems = {};
        auto exampleWarnings = SystemProblemResponseDto::createShared();
        exampleWarnings->message = "System warnings detected. See details in 'problems'.";
        auto w1 = SystemProblemDto::createShared();
        w1->type = "FirmwareVersionMismatch";
        w1->id = 1;
        w1->message = "Firmware versions differ between modules of different types (excluding core modules)";
        w1->detail = "Sensor 1.1.0 vs Control 1.3.5";
        auto w2 = SystemProblemDto::createShared();
        w2->type = "DirtyBuildFirmware";
        w2->id = 2;
        w2->message = "Dirty build firmware detected on a module";
        w2->detail = "Module sensor flagged as dirty build";
        auto w3 = SystemProblemDto::createShared();
        w3->type = "CANBusUnreachable";
        w3->id = 3;
        w3->message = "No packets observed on CAN bus (possibly unreachable)";
        w3->detail = "Interface can0 has 0 transmitted/received packets";
        auto w4 = SystemProblemDto::createShared();
        w4->type = "CANBusErrorRateHigh";
        w4->id = 4;
        w4->message = "CAN bus error rate above threshold";
        w4->detail = "Error rate at 5.4 % on CAN interface can0";
        auto w5 = SystemProblemDto::createShared();
        w5->type = "ModuleHighPing";
        w5->id = 5;
        w5->message = "High ping time detected for module";
        w5->detail = "Module control responded in 527 ms";
        exampleWarnings->problems = { w1, w2, w3, w4, w5 };

        info->addResponse<Object<SystemProblemResponseDto>>(Status::CODE_200, "application/json")
            .addExample("No Warnings", exampleOk)
            .addExample("Warnings", exampleWarnings);
    }
    ADD_CORS(getSystemWarnings)
    ENDPOINT("GET", "/system/warnings", getSystemWarnings);

    /**
    * @brief Lists all active module issues or confirms that the system is operating normally.
    */
    ENDPOINT_INFO(getModuleIssues) {
        info->summary = "Lists all active module issues or confirms normal system operation";
        info->addTag("System");
        info->description =
            "Returns all active module issues younger than 3 minutes.\n\n"
            "**Fields per issue:**\n"
            "  - `id`: Issue type ID\n"
            "  - `name`: Issue type name\n"
            "  - `index`: Additional numeric field - index, channel, etc.\n"
            "  - `timestamp`: Last occurrence time (ISO8601)\n"
            "  - `value`: Measured value related to issue\n"
            "  - `module`: Module type\n"
            "  - `instance`: Module instance";

        auto exampleOk = ModuleIssuesListDto::createShared();
        exampleOk->message = "No active module issues detected. System operating normally.";
        exampleOk->issues = {};

        auto exampleIssues = ModuleIssuesListDto::createShared();
        exampleIssues->message = "Active issues detected. See 'issues' field for details.";
        auto e1 = ModuleIssueDto::createShared();
        e1->id = 1;
        e1->name = "CoreOverTemp";
        e1->index = (short) 0;
        e1->timestamp = "2025-09-10T14:35:12";
        e1->value = 85.5f;
        e1->module = "sensor";
        e1->instance = "Exclusive";
        auto e2 = ModuleIssueDto::createShared();
        e2->id = 60;
        e2->name = "LEDPanelOverTemp";
        e2->index = 4;
        e2->timestamp = "2025-09-10T14:38:03";
        e2->value = 91.4f;
        e2->module = "control";
        e2->instance = "Exclusive";

        exampleIssues->issues = { e1, e2 };

        info->addResponse<Object<ModuleIssuesListDto>>(Status::CODE_200, "application/json")
            .addExample("No Issues", exampleOk)
            .addExample("Issues", exampleIssues);
    }
    ADD_CORS(getModuleIssues)
    ENDPOINT("GET", "/system/module/issues", getModuleIssues);

    /**
     * @brief Returns the number of received CAN packets.
     */
    ENDPOINT_INFO(getCanRxPackets) {
        info->summary = "Returns the number of received CAN packets";
        info->description = "Returns the total count of CAN packets received on the interface.";
        info->addTag("System");
        
        auto successExample = RxPacketsDto::createShared();
        successExample->rx_packets = 144640;
        info->addResponse<Object<RxPacketsDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", successExample);
        auto timeoutExample = MessageDto::createShared();
        timeoutExample->message = "Timeout occurred while retrieving CAN data";
        info->addResponse<Object<MessageDto>>(Status::CODE_408, "application/json")
            .addExample("application/json", timeoutExample);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve CAN data")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve CAN data"}}));
    }
    ADD_CORS(getCanRxPackets)
    ENDPOINT("GET", "/system/can/rx_packets", getCanRxPackets);

    /**
     * @brief Returns the number of transmitted CAN packets.
     */
    ENDPOINT_INFO(getCanTxPackets) {
        info->summary = "Returns the number of transmitted CAN packets";
        info->description = "Returns the total count of CAN packets transmitted on the interface.";
        info->addTag("System");

        auto example = TxPacketsDto::createShared();
        example->tx_packets = 132750;
        info->addResponse<Object<TxPacketsDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);

        auto timeoutExample = MessageDto::createShared();
        timeoutExample->message = "Timeout occurred while retrieving CAN data";
        info->addResponse<Object<MessageDto>>(Status::CODE_408, "application/json")
            .addExample("application/json", timeoutExample);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve CAN data")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve CAN data"}}));
    }
    ADD_CORS(getCanTxPackets)
    ENDPOINT("GET", "/system/can/tx_packets", getCanTxPackets);

    /**
     * @brief Returns the number of receive errors on the CAN interface.
     */
    ENDPOINT_INFO(getCanRxErrors) {
        info->summary = "Returns the number of receive errors on the CAN interface";
        info->description = "Returns the count of errors encountered while receiving CAN packets.";
        info->addTag("System");

        auto example = RxErrorsDto::createShared();
        example->rx_errors = static_cast<uint64_t>(0);
        info->addResponse<Object<RxErrorsDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);

        auto timeoutExample = MessageDto::createShared();
        timeoutExample->message = "Timeout while trying to retrieve CAN data";
        info->addResponse<Object<MessageDto>>(Status::CODE_408, "application/json")
            .addExample("application/json", timeoutExample);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve CAN data")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve CAN data"}}));
    }
    ADD_CORS(getCanRxErrors)
    ENDPOINT("GET", "/system/can/rx_errors", getCanRxErrors);

    /**
     * @brief Returns the number of transmit errors on the CAN interface.
     */
    ENDPOINT_INFO(getCanTxErrors) {
        info->summary = "Returns the number of transmit errors on the CAN interface";
        info->description = "Returns the count of errors encountered while transmitting CAN packets.";
        info->addTag("System");

        auto example = TxErrorsDto::createShared();
        example->tx_errors = static_cast<uint64_t>(0);
        info->addResponse<Object<TxErrorsDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);

        auto timeoutExample = MessageDto::createShared();
        timeoutExample->message = "Timeout while trying to retrieve CAN data";
        info->addResponse<Object<MessageDto>>(Status::CODE_408, "application/json")
            .addExample("application/json", timeoutExample);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve CAN data")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve CAN data"}}));
    }
    ADD_CORS(getCanTxErrors)
    ENDPOINT("GET", "/system/can/tx_errors", getCanTxErrors);

    /**
     * @brief Returns the number of received dropped packets on the CAN interface.
     */
    ENDPOINT_INFO(getCanRxDropped) {
        info->summary = "Returns the number of received dropped packets on the CAN interface";
        info->description = "Returns the count of CAN packets that were dropped on reception.";
        info->addTag("System");

        auto example = RxDroppedDto::createShared();
        example->rx_dropped = static_cast<uint64_t>(0);
        info->addResponse<Object<RxDroppedDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);

        auto timeoutExample = MessageDto::createShared();
        timeoutExample->message = "Timeout while trying to retrieve CAN data";
        info->addResponse<Object<MessageDto>>(Status::CODE_408, "application/json")
            .addExample("application/json", timeoutExample);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve CAN data")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve CAN data"}}));
    }
    ADD_CORS(getCanRxDropped)
    ENDPOINT("GET", "/system/can/rx_dropped", getCanRxDropped);

    /**
     * @brief Returns the number of transmitted dropped packets on the CAN interface.
     */
    ENDPOINT_INFO(getCanTxDropped) {
        info->summary = "Returns the number of transmitted dropped packets on the CAN interface";
        info->description = "Returns the count of CAN packets that were dropped on transmission.";
        info->addTag("System");

        auto example = TxDroppedDto::createShared();
        example->tx_dropped = static_cast<uint64_t>(0);
        info->addResponse<Object<TxDroppedDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);

        auto timeoutExample = MessageDto::createShared();
        timeoutExample->message = "Timeout while trying to retrieve CAN data";
        info->addResponse<Object<MessageDto>>(Status::CODE_408, "application/json")
            .addExample("application/json", timeoutExample);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve CAN data")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve CAN data"}}));
    }
    ADD_CORS(getCanTxDropped)
    ENDPOINT("GET", "/system/can/tx_dropped", getCanTxDropped);

    /**
     * @brief Returns the number of collisions detected on the CAN interface.
     */
    ENDPOINT_INFO(getCanCollisions){
        info->summary = "Returns the number of collisions detected on the CAN interface";
        info->description = "Returns the count of detected collisions on the CAN interface.";
        info->addTag("System");

        auto example = CollisionsDto::createShared();
        example->collisions = static_cast<uint64_t>(0);
        info->addResponse<Object<CollisionsDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);

        auto timeoutExample = MessageDto::createShared();
        timeoutExample->message = "Timeout while trying to retrieve CAN data";
        info->addResponse<Object<MessageDto>>(Status::CODE_408, "application/json")
            .addExample("application/json", timeoutExample);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve CAN data")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve CAN data"}}));
    }
    ADD_CORS(getCanCollisions)
    ENDPOINT("GET", "/system/can/collisions", getCanCollisions);

// ==========================================
// Common Endpoints
// ==========================================

    /**
     * @brief Sends a ping to the target module and returns the response time.
     */
    ENDPOINT_INFO(ping) {
        info->summary = "Send ping to target module";
        info->addTag("Common");
        info->description = "Sends ping request to target module and waits for response.";
        info->addResponse<Object<PingResponseDto>>(Status::CODE_200, "application/json", "Ping received from module");
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Module not found")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Module not found"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Ping failed")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Ping failed"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Ping timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Ping timed out"}}));
        }
    ADD_CORS(ping)
    ENDPOINT("GET", "/{module}/ping", ping, PATH(oatpp::Enum<dto::ModuleEnum>::AsString, module));

    /**
    * @brief Retrieves the CPU/MCU load of the specified module.
    */
    ENDPOINT_INFO(getCoreLoad) {
        info->summary = "Get module CPU/MCU load";
        info->addTag("Common");
        info->description = "Gets the current workload values of the computing unit. The average utilization of all available cores.";
        info->addResponse<Object<LoadResponseDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Module not found")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Module not found"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve load")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve load"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getCoreLoad)
    ENDPOINT("GET", "/{module}/load", getCoreLoad, PATH(oatpp::Enum<dto::ModuleEnum>::AsString, module));
    
    /**
     * @brief Retrieves the CPU/MCU temperature of the specified module.
     */
    ENDPOINT_INFO(getCoreTemp) {
        info->summary = "Get module CPU/MCU temperature";
        info->addTag("Common");
        info->description = "Gets the current temperature of CPU/MCU core values of the computing unit.";
        info->addResponse<Object<TempDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Module not found")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Module not found"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve temperature")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve temperature"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getCoreTemp)
    ENDPOINT("GET", "/{module}/core_temp", getCoreTemp, PATH(oatpp::Enum<dto::ModuleEnum>::AsString, module));

    /**
     * @brief Retrieves the board temperature of the specified module.
     */
    ENDPOINT_INFO(getBoardTemp) {
        info->summary = "Get module board temperature";
        info->addTag("Common");
        info->description = "Gets the current temperature of the module board, typically measured around temperature-intensive components or equipment.";
        info->addResponse<Object<TempDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Module not found")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Module not found"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve temperature")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve temperature"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getBoardTemp)
    ENDPOINT("GET", "/{module}/board_temp", getBoardTemp, PATH(oatpp::Enum<dto::ModuleEnum>::AsString, module));

    /**
    * @brief Restarts the specified module into application mode.
    */
    ENDPOINT_INFO(postRestart) {
        info->summary = "Restart module into application mode";
        info->addTag("Common");
        info->description = "This will reset the module, starting the main application firmware. Requires module UID for confirmation.";
        auto example = ModuleActionRequestDto::createShared();
        example->uid = "0x0123456789ab";
        info->addConsumes<Object<ModuleActionRequestDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Successfully restarted module")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Successfully restarted module"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Module not found")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Module not found"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Timeout while checking availability")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to restart module"}}));
    }
    ADD_CORS(postRestart)
    ENDPOINT("POST", "/{module}/restart", postRestart, PATH(oatpp::Enum<dto::ModuleEnum>::AsString, module), BODY_DTO(Object<ModuleActionRequestDto>, body));

    /**
    * @brief Reboots the specified module in USB bootloader mode.
    */
    ENDPOINT_INFO(postUsbBootloader) {
        info->summary = "Reboot module in USB bootloader mode";
        info->addTag("Common");
        info->description = 
            "This will reset the module and put it into USB bootloader mode so new firmware can be flashed via USB-C connector on board. "
            "UID of the module is required in order to confirm that correct module is selected by request.";
        auto example = ModuleActionRequestDto::createShared();
        example->uid = "0x0123456789ab";
        info->addConsumes<Object<ModuleActionRequestDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Successfully restarted module in USB bootloader mode")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Successfully restarted module in USB bootloader mode"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Module not found")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Module not found"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Timeout while checking availability")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to restart module"}}));
    }
    ADD_CORS(postUsbBootloader)
    ENDPOINT("POST", "/{module}/usb_bootloader", postUsbBootloader, 
        PATH(oatpp::Enum<dto::ModuleEnum>::AsString, module), 
        BODY_DTO(Object<ModuleActionRequestDto>, body));

    /**
    * @brief Reboots the specified module in CAN bootloader mode.
    */
    ENDPOINT_INFO(postCanBootloader) {
        info->summary = "Reboot module in CAN bootloader mode";
        info->addTag("Common");
        info->description =
            "This will reset the module and put it into CAN bootloader mode so new firmware can be flashed over CAN bus from RPi. "
            "UID of the module is required in order to confirm that correct module is selected by request.";
        auto example = ModuleActionRequestDto::createShared();
        example->uid = "0x0123456789ab";
        info->addConsumes<Object<ModuleActionRequestDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Successfully restarted module in CAN bootloader mode")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Successfully restarted module in CAN bootloader mode"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Module not found")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Module not found"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Timeout while checking availability")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to restart module"}}));
    }
    ADD_CORS(postCanBootloader)
    ENDPOINT("POST", "/{module}/can_bootloader", postCanBootloader,
        PATH(oatpp::Enum<dto::ModuleEnum>::AsString, module),
        BODY_DTO(Object<ModuleActionRequestDto>, body));

    /**
     * @brief Retrieves firmware informations.
     */
    ENDPOINT_INFO(getFwVersion) {
        info->summary = "Get module firmware version";
        info->addTag("Common");
        info->description = "Gets the current firmware version flashed on the module.\n" 
        "This is the version of the main application firmware running on the module.\n" 
        "Version is in format X.Y.Z, where X is major version and Y is minor version.\n" 
        "Z is Patch version, which is incremented with commit above major.minor version.\n" 
        "Hash is the git commit hash on which was this version built. Hash is 7-16 characters long.\n" 
        "And is generaly formatted as {:07x}, this means that longer hashes then 7 character does not have leading zeroes.\n" 
        "Dirty flag is set to true if the firmware was built from a dirty git repository (uncommitted changes).\n" 
        "Different version of firmware are compatible. Core module version numbering can be vastly different from other modules, but still follows the same pattern.\n" 
        "This is because Core module is only virtual and running on SBC (Rpi).";
        auto example = FwVersionDto::createShared();
        example->version = "1.3.5";
        example->hash = "ca123fe";
        example->dirty = false;
        info->addResponse<Object<FwVersionDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Module not found")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Module not found"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve firmware version")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve firmware version"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getFwVersion)
    ENDPOINT("GET", "/{module}/fw_version", getFwVersion,
            PATH(oatpp::Enum<dto::ModuleEnum>::AsString, module));

    /**
     * @brief Retrieves hardware version information.
     */
    ENDPOINT_INFO(getHwVersion) {
        info->summary = "Get module hardware version";
        info->addTag("Common");
        info->description = "Gets the current hardware version of the module.\n"
        "This is the revision of hardware, read directly from pcb.\n"
        "Version is in format X.Y, where X is major version and Y is minor version.\n"
        "Different hardware versions are compatible with each other and can be used in one device.";
        auto example = HwVersionDto::createShared();
        example->version = "1.2";
        info->addResponse<Object<HwVersionDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Module not found")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Module not found"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve hardware version")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve hardware version"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getHwVersion)
    ENDPOINT("GET", "/{module}/hw_version", getHwVersion,
            PATH(oatpp::Enum<dto::ModuleEnum>::AsString, module));


// ==========================================
// Core module
// ==========================================

    /**
     * @brief Retrieves the short ID (SID) of the device.
     */
    ENDPOINT_INFO(getShortID) {
        info->summary = "Get Short ID (SID) of the device";
        info->addTag("Core module");
        info->description = "Retrieves the 4-character hexadecimal SID of the device. The SID may not be unique across devices.";
        auto example = SIDDto::createShared();
        example->sid = "c0de";
        info->addResponse<Object<SIDDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve SID")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve SID"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getShortID)
    ENDPOINT("GET", "/core/sid", getShortID);

    /**
     * @brief Retrieves the IP address of the device.
     */
    ENDPOINT_INFO(getIpAddress) {
        info->summary = "Get IP address of the device";
        info->addTag("Core module");
        info->description = "Retrieves the IP address of the device. If not available, returns an empty string.";
        auto example = IpDto::createShared();
        example->ipAddress = "192.168.1.100";
        info->addResponse<Object<IpDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve IP address")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve IP address"}}));
            info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getIpAddress)
    ENDPOINT("GET", "/core/ip_address", getIpAddress);

    /**
     * @brief Retrieves the hostname of the device.
     */
    ENDPOINT_INFO(getHostname) {
        info->summary = "Get Hostname of the device";
        info->addTag("Core module");
        info->description = "Retrieves the hostname of the device, truncated to 8 characters for CAN bus compatibility.";
        auto example = HostnameDto::createShared();
        example->hostname = "smpbr_01";
        info->addResponse<Object<HostnameDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve hostname")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve hostname"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getHostname)
    ENDPOINT("GET", "/core/hostname", getHostname);

    /**
     * @brief Retrieves the serial number of the device.
     */
    ENDPOINT_INFO(getSerialNumber) {
        info->summary = "Get Serial Number of the device";
        info->addTag("Core module");
        info->description = "Retrieves the serial number of the device, which is unique and corresponds to the RPi serial number.";
        auto example = SerialDto::createShared();
        example->serial = 1907977600;
        info->addResponse<Object<SerialDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve serial number")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve serial number"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getSerialNumber)
    ENDPOINT("GET", "/core/serial", getSerialNumber);

    /**
     * @brief Retrieves the model of the Core module (SBC).
     */
    ENDPOINT_INFO(getModel) {
        info->summary = "Get model of the Core module";
        info->addTag("Core module");  
        info->description = 
                "Gets model of the Core module. Core module is based on SBC (RPi) and interface board (specific for SBC).\n"
                "Thus, the model ID is the model of the SBC.\n\n"
                "**Possible models:**\n\n"
                "- `RPi4B = Raspberry Pi 4 Model B`\n\n"
                "- `RPi3B+ = Raspberry Pi 3 Model B+`\n\n"
                "- `RPiZ2W = Raspberry Pi Zero 2 W`";
        auto example = ModelDto::createShared();
        example->model = "RPi4B";
        info->addResponse<Object<ModelDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve model")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve model"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getModel)
    ENDPOINT("GET", "/core/model", getModel);

    /**
     * @brief Retrieves the type of power supply powering the device.
     */
    ENDPOINT_INFO(getPowerSupplyType) {
        info->summary = "Get type of power supply";
        info->addTag("Core module");
        info->description = "Retrieves the type of power supply powering the device. The options are: - VIN: external power supply adapter. - PoE: Power over Ethernet from RJ45 on RPi (15W). - PoE_HB: Variant of PoE with higher power budget (25-30W).";
        auto example = SupplyTypeDto::createShared();
        example->vin = true;
        example->poe = false;
        example->poe_hb = false;
        info->addResponse<Object<SupplyTypeDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve power supply type")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve power supply type"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getPowerSupplyType)
    ENDPOINT("GET", "/core/supply/type", getPowerSupplyType);

    /**
     * @brief Retrieves the voltage of the 5V power rail.
     */
    ENDPOINT_INFO(getVoltage5V) {
        info->summary = "Get voltage of 5V power rail";
        info->addTag("Core module");
        info->description = "Gets voltage of 5V power rail on device in volts.";
        auto example = VoltageDto::createShared();
        example->voltage = 5.035;
        info->addResponse<Object<VoltageDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve voltage")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve voltage"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getVoltage5V)
    ENDPOINT("GET", "/core/supply/5v", getVoltage5V);

    /**
     * @brief Retrieves the voltage at the VIN power rail (12V).
     */
    ENDPOINT_INFO(getVoltageVIN) {
        info->summary = "Get Voltage at VIN Power Rail (12V)";
        info->addTag("Core module");
        info->description = "Retrieves the voltage at the VIN power rail on the device, which is supplied by an external power supply adapter.";
        auto example = VoltageDto::createShared();
        example->voltage = 11.995;
        info->addResponse<Object<VoltageDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve VIN voltage")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve VIN voltage"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getVoltageVIN)
    ENDPOINT("GET", "/core/supply/vin", getVoltageVIN);;

    /**
     * @brief Retrieves the voltage of PoE power rail (12V).
     */
    ENDPOINT_INFO(getPoEVoltage) {
        info->summary = "Get Voltage at PoE Power Rail (12V)";
        info->addTag("Core module");
        info->description = "Retrieves the voltage of the PoE power rail, supplied by Power over Ethernet (PoE) from RJ45 on RPi.";
        auto example = VoltageDto::createShared();
        example->voltage = 12.01;
        info->addResponse<Object<VoltageDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve POE voltage")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve POE voltage"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getPoEVoltage)
    ENDPOINT("GET", "/core/supply/poe", getPoEVoltage);

    /**
     * @brief Retrieves the current consumption of the device.
     */
    ENDPOINT_INFO(getCurrentConsumption) {
        info->summary = "Get Current Consumption of the Device";
        info->addTag("Core module");
        info->description = "Retrieves the current consumption of the device, including RPi and all connected modules. The current should be in the range 0-5 A.";
        auto example = CurrentDto::createShared();
        example->current = 0.9;
        info->addResponse<Object<CurrentDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve current consumption")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve current consumption"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getCurrentConsumption)
    ENDPOINT("GET", "/core/supply/current", getCurrentConsumption);

    /**
     * @brief Retrieves the power draw of the device in watts.
     */
    ENDPOINT_INFO(getPowerDraw) {
        info->summary = "Get Power Draw of the Device";
        info->addTag("Core module");
        info->description = "Retrieves the power draw of the device, including RPi and all connected modules.";
        auto example = PowerDrawDto::createShared();
        example->power_draw = 4.2;
        info->addResponse<Object<PowerDrawDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve power draw")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve power draw"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getPowerDraw)
    ENDPOINT("GET", "/core/supply/power_draw", getPowerDraw);




// ==========================================
// Control module
// ==========================================
    /**
    * @brief Sets all channels of LED panel to given intensity.
    */
    ENDPOINT_INFO(setIntensities) {
        info->summary = "Sets all channels of LED panel to given intensity";
        info->description = "In format: {\"intensity\": [0.5,1,0,0.2] }";
        info->addTag("Control module");  

        auto example = IntensitiesDto::createShared();
        auto intensityValues = std::vector<oatpp::Float32>{0.5f, 1.0f, 0.0f, 0.2f};
        example->intensity = oatpp::Vector<oatpp::Float32>::createShared();
        example->intensity->assign(intensityValues.begin(), intensityValues.end());
  
        info->addConsumes<Object<IntensitiesDto>>("application/json")
            .addExample("application/json", example);  
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Intensity set successfully")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Intensity set successfully"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Invalid intensity value")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Invalid intensity value"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to set intensity")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to set intensity"}}));
    }
    ADD_CORS(setIntensities)
    ENDPOINT("POST", "/control/led_panel/intensity", setIntensities, BODY_DTO(Object<IntensitiesDto>, body));      

    /**
    * @brief Sets the intensity and the channel of the LED lighting.
    */
    ENDPOINT_INFO(setIntensity) {
        info->summary = "Set the intensity and the channel of LED lighting.";
        info->description = "This endpoint allows the user to set the intensity of the LED lighting and the channel. "
                            "The intensity value should be a float between 0 and 1, "
                            "where 0 represents off and 1 represents maximum brightness. "
                            "The channel value must be one of the following: 'channel0', 'channel1', 'channel2', or 'channel3', "
                            "representing the specific LED channel to control.";
        info->addTag("Control module");
        auto example = IntensityDto::createShared();
        example->intensity = 0.5;
        info->addConsumes<Object<IntensityDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Intensity set successfully")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Intensity set successfully"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Invalid intensity value or channel value")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Invalid intensity value or channel value"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to set intensity")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to set intensity"}}));
    }
    ADD_CORS(setIntensity)
    ENDPOINT("POST", "/control/led_panel/intensity/{channel}", setIntensity, PATH(oatpp::Enum<dto::ChannelEnum>::AsString, channel), BODY_DTO(Object<IntensityDto>, body));

    /**
    * @brief Retrieves the current intensity of the selected LED channel.
    */
    ENDPOINT_INFO(getIntensity) {
        info->summary = "Retrieves current intensity of selected channel of LED panel";
        info->description = "Returns the current intensity value for a given channel.";
        info->addTag("Control module");
        
        auto example = IntensityDto::createShared();
        example->intensity = 0.5;
        info->addResponse<Object<IntensityDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Channel not found")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Channel not found"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve intensity")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve intensity"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getIntensity)
    ENDPOINT("GET", "/control/led_panel/intensity/{channel}", getIntensity, PATH(oatpp::Enum<dto::ChannelEnum>, channel));

    /**
    * @brief Retrieves the temperature of the LED panel.
    */
    ENDPOINT_INFO(getLedTemperature) {
        info->summary = "Get LED panel temperature";
        info->description = "Retrieves the current temperature of the LED panel in °C.";
        info->addTag("Control module");
        auto example = TempDto::createShared();
        example->temperature = 30.2;
        info->addResponse<Object<TempDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "LED panel not available")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "LED panel not available"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve LED temperature")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve LED temperature"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getLedTemperature)
    ENDPOINT("GET", "/control/led_panel/temperature", getLedTemperature);

    /**
     * @brief Sets the intensity of heating or cooling.
     */
    ENDPOINT_INFO(setHeaterIntensity) {
        info->summary = "Set heater intensity";
        info->description = "Sets the intensity of heating or cooling in range -1.0 (cooling) to 1.0 (heating). "
                             "This value can be overwritten by the regulation algorithm if temperature regulation (target temperature) is set.";
        info->addTag("Control module");
        auto example = IntensityDto::createShared();
        example->intensity = -0.33; 
        info->addConsumes<Object<IntensityDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Intensity set successfully")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Intensity set successfully"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Invalid intensity value")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Invalid intensity value"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to set intensity")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to set intensity"}}));
    }
    ADD_CORS(setHeaterIntensity)
    ENDPOINT("POST", "/control/heater/intensity", setHeaterIntensity, BODY_DTO(Object<IntensityDto>, body));

    /**
     * @brief Retrieves the current intensity of heating or cooling.
     */
    ENDPOINT_INFO(getHeaterIntensity) {
        info->summary = "Get heater intensity";
        info->description = "Retrieves current intensity of heating or cooling. Range is -1.0 (cooling) to 1.0 (heating). "
                             "Intensity can be modified by regulation algorithm if temperature regulation (target temperature) is set.";
        info->addTag("Control module"); 
        auto example = IntensityDto::createShared();
        example->intensity = -0.33;
        info->addResponse<Object<IntensityDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve heater intensity")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve heater intensity"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getHeaterIntensity)
    ENDPOINT("GET", "/control/heater/intensity", getHeaterIntensity);

    /**
     * @brief Sets the target temperature for the heater.
     */
    ENDPOINT_INFO(setHeaterTargetTemperature) {
        info->summary = "Set heater target temperature";
        info->description = "Sets the target temperature for the heater (temperature of bottle) in ˚C. "
                             "The heater will adjust its intensity to reach this temperature.";
        info->addTag("Control module");
        auto example = TempDto::createShared();
        example->temperature = 30.5; 
        info->addConsumes<Object<TempDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Target temperature set successfully")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Target temperature set successfully"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Invalid target temperature")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Invalid target temperature"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to set target temperature")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to set target temperature"}}));
    }
    ADD_CORS(setHeaterTargetTemperature)
    ENDPOINT("POST", "/control/heater/target_temperature", setHeaterTargetTemperature, BODY_DTO(Object<TempDto>, body));

    /**
     * @brief Retrieves the currently set target temperature for the heater.
     */
    ENDPOINT_INFO(getHeaterTargetTemperature) {
        info->summary = "Get heater target temperature";
        info->description = "Retrieves the currently set target temperature for the heater (temperature of bottle) in °C.";
        info->addTag("Control module");
        
        auto example = TempDto::createShared();
        example->temperature = 30.5; 
        info->addResponse<Object<TempDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Target temperature is not set")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Target temperature is not set"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve heater target temperature")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve heater target temperature"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getHeaterTargetTemperature)
    ENDPOINT("GET", "/control/heater/target_temperature", getHeaterTargetTemperature);

    /**
     * @brief Retrieves the current temperature of the heater plate (metal heatspreader).
     */
    ENDPOINT_INFO(getHeaterPlateTemperature) {
        info->summary = "Get heater plate temperature";
        info->description = "Retrieves temperature of plate (metal heatspreader) which is controlling temperature of bottle. "
                             "Sensor is thermistor connected from back side of heater plate.";
        info->addTag("Control module");
        auto example = TempDto::createShared();
        example->temperature = 30.5; 
        info->addResponse<Object<TempDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Heater plate not available")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Heater plate not available"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve heater plate temperature")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve heater plate temperature"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getHeaterPlateTemperature)
    ENDPOINT("GET", "/control/heater/plate_temperature", getHeaterPlateTemperature);

    /**
     * @brief Turns off the heater.
     */
    ENDPOINT_INFO(turnOffHeater) {
        info->summary = "Turn off heater";
        info->description = "Turns off the heater by setting intensity to 0.0 and disabling temperature regulation.";
        info->addTag("Control module");
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Heater was turned off")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Heater was turned off"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to turn off heater")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to turn off heater"}}));
    }
    ADD_CORS(turnOffHeater)
    ENDPOINT("GET", "/control/heater/turn_off", turnOffHeater);

    /**
     * @brief Retrieves information about the cuvette pump.
     */
    ENDPOINT_INFO(getCuvettePumpInfo) {
        info->summary = "Retrieves information about the cuvette pump";
        info->description = "Units: ml/min. Retrieves information about the cuvette pump capabilities.\n"  
        "This includes maximum and minimum flowrate of the cuvette pump at nominal conditions.\n"  
        "Cuvette pump maximal and minimal flowrates can be different based on used tubing.";
        info->addTag("Control module");
        auto example = CuvettePumpInfoDto::createShared();
        example->max_flowrate = 200.0f;
        example->min_flowrate = 10.0f;
        info->addResponse<Object<CuvettePumpInfoDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve pump info")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve pump info"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getCuvettePumpInfo)
    ENDPOINT("GET", "/control/cuvette_pump/info", getCuvettePumpInfo);



    /**
     * @brief Sets the speed of the cuvette pump.
     */
    ENDPOINT_INFO(setCuvettePumpSpeed) {
        info->summary = "Set cuvette pump speed";
        info->description = "Sets the speed of the cuvette pump in the range -1.0 (pumping out) to 1.0 (pumping in).";
        info->addTag("Control module");
        auto example = SpeedDto::createShared();
        example->speed = -0.33;
        info->addConsumes<Object<SpeedDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Speed set successfully")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Speed set successfully"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Invalid speed value")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Invalid speed value"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to set pump speed")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to set pump speed"}}));
    }
    ADD_CORS(setCuvettePumpSpeed)
    ENDPOINT("POST", "/control/cuvette_pump/speed", setCuvettePumpSpeed, BODY_DTO(Object<SpeedDto>, body));

    /**
     * @brief Retrieves the current speed of the cuvette pump.
     */
    ENDPOINT_INFO(getCuvettePumpSpeed) {
        info->summary = "Get cuvette pump speed";
        info->description = "Retrieves the current speed of the cuvette pump. The range is -1.0 (pumping out) to 1.0 (pumping in).";
        info->addTag("Control module");
        auto example = SpeedDto::createShared();
        example->speed = -0.33;
        info->addResponse<Object<SpeedDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve pump speed")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve pump speed"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getCuvettePumpSpeed)
    ENDPOINT("GET", "/control/cuvette_pump/speed", getCuvettePumpSpeed);

    /**
     * @brief Sets flowrate of the cuvette pump
     */
    ENDPOINT_INFO(setCuvettePumpFlowrate) {
        info->summary = "Set cuvette pump flowrate";
        info->description = "Sets the flowrate of the cuvette pump in the range -1000.0 (pumping out) to 1000.0 (pumping in).";
        info->addTag("Control module");
        auto example = FlowrateDto::createShared();
        example->flowrate = 10; 
        info->addConsumes<Object<FlowrateDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Flowrate set successfully")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Flowrate set successfully"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Invalid flowrate value")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Invalid flowrate value"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to set flowrate")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to set flowrate"}}));
    }
    ADD_CORS(setCuvettePumpFlowrate)
    ENDPOINT("POST", "/control/cuvette_pump/flowrate", setCuvettePumpFlowrate, BODY_DTO(Object<FlowrateDto>, body));

    /**
     * @brief Retrieves current flowrate of the cuvette pump
     */
    ENDPOINT_INFO(getCuvettePumpFlowrate) {
        info->summary = "Get cuvette pump flowrate";
        info->description = "Retrieves the current flowrate of the cuvette pump. Positive values indicate pumping in, and negative values indicate pumping out.";
        info->addTag("Control module");
        auto example = FlowrateDto::createShared();
        example->flowrate = 10;
        info->addResponse<Object<FlowrateDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve flowrate")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve flowrate"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getCuvettePumpFlowrate)
    ENDPOINT("GET", "/control/cuvette_pump/flowrate", getCuvettePumpFlowrate);

    /**
     * @brief Moves requested amount of liquid in or out of the cuvette
     */
    ENDPOINT_INFO(moveCuvettePump) {
        info->summary = "Move cuvette pump liquid";
        info->description = "Moves the requested amount of liquid in or out of the cuvette. Positive volume means pumping in, and negative volume means pumping out. Flowrate is specified in ml/min and must be positive. If set to zero, the maximum flowrate of the pump will be used.";
        info->addTag("Control module");
    
        auto example = MoveDto::createShared();
        example->volume = 10; 
        example->flowrate = 2; 
        info->addConsumes<Object<MoveDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Movement started successfully")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Movement started successfully"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Invalid volume or flowrate value")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Invalid volume or flowrate value"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to start movement")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to start movement"}}));
    }
    ADD_CORS(moveCuvettePump)
    ENDPOINT("POST", "/control/cuvette_pump/move", moveCuvettePump, BODY_DTO(Object<MoveDto>, body));

    /**
     * @brief Primes the cuvette pump.
     */
    /*ENDPOINT_INFO(primeCuvettePump) {
        info->summary = "Prime cuvette pump";
        info->description = "Primes the cuvette pump by pumping liquid into the cuvette. This is used to fill the cuvette with liquid and remove air from the system.";
        info->addTag("Control module");
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Cuvette pump priming was started")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Cuvette pump priming was started"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to start cuvette pump priming")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to start cuvette pump priming"}}));
    }
    ADD_CORS(primeCuvettePump)
    ENDPOINT("POST", "/control/cuvette_pump/prime", primeCuvettePump);*/

    /**
     * @brief Purges the cuvette pump.
     */
    /*ENDPOINT_INFO(purgeCuvettePump) {
        info->summary = "Purge cuvette pump";
        info->description = "Purges the cuvette pump by pumping liquid out of the cuvette. This is used to remove liquid from the cuvette and clean the system.";
        info->addTag("Control module");
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Cuvette pump purging was started")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Cuvette pump purging was started"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to start cuvette pump purging")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to start cuvette pump purging"}}));
    }
    ADD_CORS(purgeCuvettePump)
    ENDPOINT("POST", "/control/cuvette_pump/purge", purgeCuvettePump);*/

    /**
     * @brief Stops the cuvette pump.
     */
    ENDPOINT_INFO(stopCuvettePump) {
        info->summary = "Stop cuvette pump";
        info->description = "Stops the cuvette pump immediately and disables any planned movements.";
        info->addTag("Control module");
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Cuvette pump was stopped")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Cuvette pump was stopped"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to stop cuvette pump")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to stop cuvette pump"}}));
    }
    ADD_CORS(stopCuvettePump)
    ENDPOINT("GET", "/control/cuvette_pump/stop", stopCuvettePump);

    /**
     * @brief Configures the maximum flowrate calibration value for the cuvette pump.
     */
    ENDPOINT_INFO(calibrateCuvettePump) {
        info->summary = "Configure measured volume value on module in order to calibrate cuvette pump";
        info->description = 
            "Sends calibrated value of flowrate per minute (ml/min) to pump in order to calibrate move and flowrate commands.\n"
            "Calibration should be used if it is necessary to achieve a dosing accuracy greater than 5% or if the liquid has a non-standard viscosity.\n"
            "Value of calibration can be checked by using GET request to info endpoint of cuvette pump.\n"
            "Calibration send by this endpoint is stored on module and used after restart.\n"
            "Calibration procedure:\n"
            "- Insert the inlet tube of the cuvette pump into a container filled with liquid.\n"
            "- Place the outlet tube into a measuring cylinder.\n"
            "- Start the pump so that the entire circuit is filled with liquid (primed).\n"
            "- Empty measuring cylinder.\n"
            "- Run the pump at maximum speed for exactly one minute (speed 1.0).\n"
            "- Measure the volume of liquid dispensed into the measuring cylinder.\n"
            "- Send measured volume / flowrate to the module. Unit is ml/min.\n"
            "Common value is around 10 to 100 ml/min, default is 30 ml/min.";
        info->addTag("Control module");
        auto example = FlowrateDto::createShared();
        example->flowrate = 30.0;
        info->addConsumes<Object<FlowrateDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Successfully wrote cuvette pump max flowrate calibration")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Successfully wrote cuvette pump max flowrate calibration"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to set calibration")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to set calibration"}}));
    }
    ADD_CORS(calibrateCuvettePump)
    ENDPOINT("POST", "/control/cuvette_pump/calibration", calibrateCuvettePump, BODY_DTO(Object<FlowrateDto>, body));

    /**
     * @brief Retrieves capabilities of the aerator (air pump).
     */
    ENDPOINT_INFO(getAeratorInfo) {
        info->summary = "Retrieves information about the aerator (air pump)";
        info->description = "Units: ml/min. Retrieves information about the aerator capabilities.\n"
            "This includes maximum and minimum flowrate of the aerator at nominal conditions.\n"
            "Aerator maximal and minimal flowrates can be different based on used tubing and their lengths.";
        info->addTag("Control module");
        auto example = AeratorInfoDto::createShared();
        example->max_flowrate = 2500.0f;
        example->min_flowrate = 10.0f;
    
        info->addResponse<Object<AeratorInfoDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve aerator info")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve aerator info"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getAeratorInfo)
    ENDPOINT("GET", "/control/aerator/info", getAeratorInfo);


    /**
     * @brief Sets the speed of the aerator.
     */
    ENDPOINT_INFO(setAeratorSpeed) {
        info->summary = "Set aerator speed";
        info->description = "Sets the speed of the aerator in range 0.0 to 1.0.";
        info->addTag("Control module");
        auto example = SpeedDto::createShared();
        example->speed = 0.5;
        info->addConsumes<Object<SpeedDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Speed set successfully")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Speed set successfully"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Invalid speed value")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Invalid speed value"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to set aerator speed")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to set aerator speed"}}));
    }
    ADD_CORS(setAeratorSpeed)
    ENDPOINT("POST", "/control/aerator/speed", setAeratorSpeed, BODY_DTO(Object<SpeedDto>, body));

    /**
     * @brief Retrieves the current speed of the aerator.
     */
    ENDPOINT_INFO(getAeratorSpeed) {
        info->summary = "Get aerator speed";
        info->description = "Retrieves current speed of the aerator. Range is 0.0 to 1.0.";
        info->addTag("Control module");
        auto example = SpeedDto::createShared();
        example->speed = 0.5;
        info->addResponse<Object<SpeedDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve aerator speed")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve aerator speed"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getAeratorSpeed)
    ENDPOINT("GET", "/control/aerator/speed", getAeratorSpeed);

    /**
     * @brief Sets the flowrate of the aerator.
     */
    ENDPOINT_INFO(setAeratorFlowrate) {
        info->summary = "Set aerator flowrate";
        info->description = "Sets the flowrate of the aerator in range 0.0 to 5000.0 ml/min.";
        info->addTag("Control module");
        auto example = FlowrateDto::createShared();
        example->flowrate = 100;
        info->addConsumes<Object<FlowrateDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Flowrate set successfully")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Flowrate set successfully"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Invalid flowrate value")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Invalid flowrate value"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to set aerator flowrate")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to set aerator flowrate"}}));
    }
    ADD_CORS(setAeratorFlowrate)
    ENDPOINT("POST", "/control/aerator/flowrate", setAeratorFlowrate, BODY_DTO(Object<FlowrateDto>, body));

    /**
     * @brief Retrieves the current flowrate of the aerator.
     */
    ENDPOINT_INFO(getAeratorFlowrate) {
        info->summary = "Get aerator flowrate";
        info->description = "Retrieves current flowrate of the aerator. Range is 0.0 to 5000.0 ml/min.";
        info->addTag("Control module");
        auto example = FlowrateDto::createShared();
        example->flowrate = 100;
        info->addResponse<Object<FlowrateDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve aerator flowrate")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve aerator flowrate"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getAeratorFlowrate)
    ENDPOINT("GET", "/control/aerator/flowrate", getAeratorFlowrate);;

    /**
     * @brief Moves requested amount of air into the bottle using the aerator
     */
    ENDPOINT_INFO(moveAerator) {
        info->summary = "Move aerator air";
        info->description = "Moves the requested amount of air into the bottle. Volume is in ml. Flowrate is in ml/min. If set to zero, the maximum flowrate of the pump will be used.";
        info->addTag("Control module");
        auto example = MoveDto::createShared();
        example->volume = 100;
        example->flowrate = 100;
        info->addConsumes<Object<MoveDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Movement started successfully")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Movement started successfully"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Invalid volume or flowrate value")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Invalid volume or flowrate value"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to start movement")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to start movement"}}));
    }
    ADD_CORS(moveAerator)
    ENDPOINT("POST", "/control/aerator/move", moveAerator, BODY_DTO(Object<MoveDto>, body));

    /**
     * @brief Stops the aerator and disables any planned movements.
     */
    ENDPOINT_INFO(stopAerator) {
        info->summary = "Stop aerator";
        info->description = "Stops the aerator immediately and disables any planned movements.";
        info->addTag("Control module");
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Aerator was stopped")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Aerator was stopped"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to stop aerator")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to stop aerator"}}));
    }
    ADD_CORS(stopAerator)
    ENDPOINT("GET", "/control/aerator/stop", stopAerator);

    /**
     * @brief Retrieves capabilities of the mixer.
     */
    ENDPOINT_INFO(getMixerInfo) {
        info->summary = "Retrieves information about the mixer";
        info->description = "Units: revolutions per minute (RPM). Retrieves information about the mixer capabilities.\n"
            "This includes maximum and minimum reliable RPM of the mixer at normal conditions.\n"
            "Mixer maximal and minimal speeds can be different based on used magnetic stirrer and liquid density.\n"
            "Lower then minimal values can be set.";
        info->addTag("Control module");
        auto example = MixerInfoDto::createShared();
        example->max_rpm = 5000;
        example->min_rpm = 1000;

        info->addResponse<Object<MixerInfoDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve mixer info")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve mixer info"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getMixerInfo)
    ENDPOINT("GET", "/control/mixer/info", getMixerInfo);

    /**
     * @brief Sets the speed of the mixer.
     */
    ENDPOINT_INFO(setMixerSpeed) {
        info->summary = "Set mixer speed";
        info->description = "Sets the speed of the mixer in the range [0.0, 1.0], where 0.0 is stopped and 1.0 is maximum speed.";
        info->addTag("Control module");
        auto example = SpeedDto::createShared();
        example->speed = 0.5; 
        info->addConsumes<Object<SpeedDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Speed set successfully")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Speed set successfully"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Invalid speed value")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Invalid speed value"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to set mixer speed")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to set mixer speed"}}));
    }
    ADD_CORS(setMixerSpeed)
    ENDPOINT("POST", "/control/mixer/speed", setMixerSpeed, BODY_DTO(Object<SpeedDto>, body));

    /**
     * @brief Retrieves the current speed of the mixer.
     */
    ENDPOINT_INFO(getMixerSpeed) {
        info->summary = "Get mixer speed";
        info->description = "Retrieves the current speed of the mixer in the range [0.0, 1.0], where 0.0 is stopped and 1.0 is maximum speed.";
        info->addTag("Control module");
        auto example = SpeedDto::createShared();
        example->speed = 0.5; 
        info->addResponse<Object<SpeedDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve mixer speed")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve mixer speed"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getMixerSpeed)
    ENDPOINT("GET", "/control/mixer/speed", getMixerSpeed);

    /**
     * @brief Sets the target RPM of the mixer.
     */
    ENDPOINT_INFO(setMixerRpm) {
        info->summary = "Set mixer RPM";
        info->description = "Sets the target RPM of the mixer. The range is 0 to 10000 RPM.";
        info->addTag("Control module");
        auto example = RpmDto::createShared();
        example->rpm = 3000; 
        info->addConsumes<Object<RpmDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Target RPM set successfully")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Target RPM set successfully"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Invalid RPM value")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Invalid RPM value"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to set mixer RPM")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to set mixer RPM"}}));
    }
    ADD_CORS(setMixerRpm)
    ENDPOINT("POST", "/control/mixer/rpm", setMixerRpm, BODY_DTO(Object<RpmDto>, body));

    /**
     * @brief Retrieves the current RPM of the mixer.
     */
    ENDPOINT_INFO(getMixerRpm) {
        info->summary = "Get mixer RPM";
        info->description = "Retrieves the current RPM of the mixer. The range is 0 to 10000 RPM.";
        info->addTag("Control module");
        auto example = RpmDto::createShared();
        example->rpm = 500;
        info->addResponse<Object<RpmDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve mixer RPM")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve mixer RPM"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getMixerRpm)
    ENDPOINT("GET", "/control/mixer/rpm", getMixerRpm);

    /**
     * @brief Sets the mixer to stir at a specified RPM for a specified time.
     */
    ENDPOINT_INFO(stirMixer) {
        info->summary = "Stir mixer";
        info->description = "Sets the mixer to stir at a specified RPM for a specified time. RPM is in RPM, and time is in seconds.";
        info->addTag("Control module");
        auto example = StirDto::createShared();
        example->rpm = 3000; 
        example->time = 60; 
        info->addConsumes<Object<StirDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Stirring started successfully")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Stirring started successfully"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Invalid RPM or time value")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Invalid RPM or time value"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to start stirring")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to start stirring"}}));
    }
    ADD_CORS(stirMixer)
    ENDPOINT("POST", "/control/mixer/stir", stirMixer, BODY_DTO(Object<StirDto>, body));

    /**
     * @brief Stops the mixer immediately.
     */
    ENDPOINT_INFO(stopMixer) {
        info->summary = "Stop mixer";
        info->description = "Stops the mixer immediately and disables any planned movements.";
        info->addTag("Control module");
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Mixer was stopped")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Mixer was stopped"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to stop the mixer")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to stop the mixer"}}));
    }
    ADD_CORS(stopMixer)
    ENDPOINT("GET", "/control/mixer/stop", stopMixer);

// ==========================================
// Control module
// ==========================================
    /**
     * @brief Retrieves the temperature of the bottle.
     */
    ENDPOINT_INFO(getBottleTemperature) {
        info->summary = "Retrieves temperature of the bottle";
        info->description = "Retrieves the temperature of the bottle in °C.";
        info->addTag("Sensor module");
        auto example = TempDto::createShared();
        example->temperature = 30.2; 
        info->addResponse<Object<TempDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Bottle temperature not available")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Bottle temperature not available"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve temperature")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve temperature"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getBottleTemperature)
    ENDPOINT("GET", "/sensor/bottle/temperature", getBottleTemperature);

    /**
     * @brief Retrieves the measured temperature from the top sensor of the bottle.
     */
    ENDPOINT_INFO(getTopMeasuredTemperature) {
        info->summary = "Retrieves measured temperature from top sensor";
        info->description = "Retrieves the measured temperature from the top sensor of the bottle in °C.";
        info->addTag("Sensor module");
        auto example = TempDto::createShared();
        example->temperature = 30.2; 
        info->addResponse<Object<TempDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Top temperature not available")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Top temperature not available"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve temperature")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve temperature"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getTopMeasuredTemperature)
    ENDPOINT("GET", "/sensor/bottle/top/measured_temperature", getTopMeasuredTemperature);

    /**
     * @brief Retrieves the measured temperature from the bottom sensor of the bottle.
     */
    ENDPOINT_INFO(getBottomMeasuredTemperature) {
        info->summary = "Retrieves measured temperature from bottom sensor";
        info->description = "Retrieves the measured temperature from the bottom sensor of the bottle in °C.";
        info->addTag("Sensor module"); 
        auto example = TempDto::createShared();
        example->temperature = 30.2; 
        info->addResponse<Object<TempDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Bottom temperature not available")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Bottom temperature not available"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve temperature")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve temperature"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getBottomMeasuredTemperature)
    ENDPOINT("GET", "/sensor/bottle/bottom/measured_temperature", getBottomMeasuredTemperature);

    /**
     * @brief Retrieves the temperature of the top sensor case of the bottle.
     */
    ENDPOINT_INFO(getTopSensorTemperature) {
        info->summary = "Retrieves temperature of the top sensor case";
        info->description = "Retrieves the temperature of the sensor case on top of the bottle in °C.";
        info->addTag("Sensor module");
        auto example = TempDto::createShared();
        example->temperature = 30.2; 
        info->addResponse<Object<TempDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Top sensor temperature not available")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Top sensor temperature not available"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve temperature")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve temperature"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getTopSensorTemperature)
    ENDPOINT("GET", "/sensor/bottle/top/sensor_temperature", getTopSensorTemperature);

    /**
     * @brief Retrieves the temperature of the bottom sensor case of the bottle.
     */
    ENDPOINT_INFO(getBottomSensorTemperature) {
        info->summary = "Retrieves temperature of the bottom sensor case";
        info->description = "Retrieves the temperature of the sensor case on the bottom of the bottle in °C.";
        info->addTag("Sensor module");
        auto example = TempDto::createShared();
        example->temperature = 30.2; 
        info->addResponse<Object<TempDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Bottom sensor temperature not available")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Bottom sensor temperature not available"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve temperature")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve temperature"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getBottomSensorTemperature)
    ENDPOINT("GET", "/sensor/bottle/bottom/sensor_temperature", getBottomSensorTemperature);

    /**
     * @brief Clears custom text on Mini OLED display and displays the serial number.
     */
    ENDPOINT_INFO(clearCustomText) {
        info->summary = "Clear custom text on Mini OLED display";
        info->description = "Clear custom text on Mini OLED display and display serial number of the device.";
        info->addTag("Sensor module");
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Successfully cleared custom text on Mini OLED display")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Successfully cleared custom text on Mini OLED display"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to clear custom text")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to clear custom text"}}));
    }
    ADD_CORS(clearCustomText)
    ENDPOINT("GET", "/sensor/oled/clear_custom_text", clearCustomText);

    /**
     * @brief Prints custom text on Mini OLED display.
     */
    ENDPOINT_INFO(printCustomText) {
        info->summary = "Print custom text on Mini OLED display";
        info->description = "Use last line of Mini OLED display to print custom text. Text will be appended to existing text.";
        info->addTag("Sensor module");
        auto example = TextDto::createShared();
        example->text = "text to printed"; 
        info->addConsumes<Object<TextDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Successfully printed custom text on Mini OLED display")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Successfully printed custom text on Mini OLED display"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to print custom text")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to print custom text"}}));
    }
    ADD_CORS(printCustomText)
    ENDPOINT("POST", "/sensor/oled/print_custom_text", printCustomText, BODY_DTO(Object<TextDto>, dto));

    /**
     * @brief Perform single sample measurement on fluorometer.
     */
    ENDPOINT_INFO(performFluorometerSingleSample) {
        info->summary = "Perform single sample measurement on fluorometer";
        info->description = 
            "Perform single sample of data measurement on fluorometer.\n"
            "This will start capturing of single sample and immediately retrieve it.\n"
            "Detector gain and emitor intensity can be used to change relative range of measured values.\n"
            "Emitor intensity is not strictly linear, so it is better to use gain of detector to change range.\n"
            "Absolute value is calculated based on gain of detector and emitor intensity. And relates to detector max range.\n"
            "\n"
            "Allowed values:\n"
            "- detector_gain: \"x1\", \"x10\", \"x50\", \"Auto\"\n";
        info->addTag("Sensor module");

        auto exampleRequest = FluorometerSingleSampleRequestDto::createShared();
        exampleRequest->emitor_intensity = 0.5f;
        exampleRequest->detector_gain = "x1";
        
        auto exampleResponse = FluorometerSingleSampleResponseDto::createShared();
        exampleResponse->raw_value = 1023;
        exampleResponse->relative_value = 0.25f;
        exampleResponse->absolute_value = 0.125f;

        info->addConsumes<Object<FluorometerSingleSampleRequestDto>>("application/json")
            .addExample("application/json", exampleRequest);
        info->addResponse<Object<FluorometerSingleSampleResponseDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", exampleResponse);
    }
    ADD_CORS(performFluorometerSingleSample)
    ENDPOINT("POST", "/sensor/fluorometer/single_sample", performFluorometerSingleSample, 
        BODY_DTO(Object<FluorometerSingleSampleRequestDto>, body));

    /**
     * @brief Perform OJIP capture on fluorometer.
     */
    ENDPOINT_INFO(captureFluorometerOjip) {
        info->summary = "Perform OJIP capture on fluorometer";
        info->description = 
            "Start OJIP capture on fluorometer and return captured data.\n"
            "This will start capturing the OJIP curve and store it in the memory of the fluorometer.\n"
            "After the capture is completed, it will read the data and return it.\n"
            "During measurement, some components are unable to be used, like density measurements, and their processing can be postponed.\n"
            "Detector gain and emitter intensity can be used to change the relative range of measured values.\n"
            "This can lead to better relative resolution, with regard to the max value of the OJIP curve.\n"
            "But this can also lead to saturation of the detector and loss of data.\n"
            "Emitter intensity is not strictly linear, so it is better to use the gain of the detector to change the range.\n"
            "Absolute value is calculated based on the gain of the detector and emitter intensity. And relates to the detector's max range.\n"
            "More info about the returned data can be found in the read_last endpoint description.\n"
            "Expected timeout can be estimated as:\n"
            "timeout_ms ≈ length_ms + 2 × required_samples (in milliseconds).\n"
            "\n"
            "Allowed values:\n"
            "- detector_gain: \"x1\", \"x10\", \"x50\", \"Auto\"\n"
            "- timebase: \"linear\", \"logarithmic\"\n";
        info->addTag("Sensor module");

        auto example = FluorometerMeasurementDto::createShared();
        example->measurement_id = 1235;
        example->read = false;
        example->saturated = false;
        example->detector_gain = dto::GainEnum::x1;
        example->emitor_intensity = 0.5;
        example->timebase = dto::TimebaseEnum::logarithmic;
        example->timestamp = "2025-05-30T12:34:56";
        example->length_ms = 1000;
        example->required_samples = 1000;
        example->captured_samples = 998;
        example->missing_samples = 2;

        
        auto sample1 = FluorometerSampleDto::createShared();
        sample1->time_ms = 1.2f; 
        sample1->raw_value = 1023; 
        sample1->relative_value = 0.5f; 
        sample1->absolute_value = 0.125f;

        auto sample2 = FluorometerSampleDto::createShared();
        sample2->time_ms = 10.0f;
        sample2->raw_value = 4095;
        sample2->relative_value = 1.0f;
        sample2->absolute_value = 0.95f;

        auto example2 = FluorometerOjipCaptureRequestDto::createShared();
        example2->detector_gain = "x1";
        example2->emitor_intensity = 1.0;
        example2->timebase = "logarithmic";
        example2->length_ms = 1000;
        example2->sample_count = 1000;

        example->samples = {sample1, sample2};

        info->addConsumes<Object<FluorometerOjipCaptureRequestDto>>("application/json")
            .addExample("application/json", example2);
        info->addResponse<Object<FluorometerMeasurementDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "OJIP data not available")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "OJIP data not available"}}));
    }

    ADD_CORS(captureFluorometerOjip)
    ENDPOINT("POST", "/sensor/fluorometer/ojip/capture", captureFluorometerOjip,
        BODY_DTO(Object<FluorometerOjipCaptureRequestDto>, body));

    /**
     * @brief Checks if fluorometer OJIP capture is complete.
     */
    ENDPOINT_INFO(checkFluorometerOjipCaptureComplete) {
        info->summary = "Check if fluorometer OJIP capture is complete";
        info->description = "Check if fluorometer capture is complete and data is ready to be retrieved.";
        info->addTag("Sensor module");
        auto example = FluorometerCaptureStatusDto::createShared();
        example->capture_complete = false; 
        info->addResponse<Object<FluorometerCaptureStatusDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Fluorometer capture status not available")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Fluorometer capture status not available"}}));
    }

    ADD_CORS(checkFluorometerOjipCaptureComplete)
    ENDPOINT("GET", "/sensor/fluorometer/ojip/completed", checkFluorometerOjipCaptureComplete);

/**
     * @brief Retrieve OJIP data from fluorometer.
     */
    ENDPOINT_INFO(retrieveLastFluorometerOjipData) {
        info->summary = "Retrieve OJIP data from last fluorometer capture";
        info->description = 
            "Retrieve OJIP data from last fluorometer capture.\n"
            "Measurement ID is generally increasing in value and different for every new measurement started.\n"
            "If measurement is retrieved twice, ID will be the same.\n"
            "Measurement ID is used to determine if this measurement is already exported to the database.\n"
            "Boolean Read contains information if data was already read from the sensor.\n"
            "It starts as true and resets with every new measurement started.\n"
            "Serves as an easier way to determine if data was already read instead of Measurement ID.\n"
            "The second retrieve without starting a new measurement will return false.\n"
            "Samples are always ordered by the time of capture.\n"
            "Time between capture_start and retrieve should be at least double the length of the measurement.\n"
            "Based on selected parameters when the measurement was started, data can be saturated.\n"
            "Depending on the gain of the detector or intensity of the emitter, adjustments can be made for the next measurement.\n"
            "Intensity of the emitter is not strictly linear, so it is better to use the gain of the detector to change absolute values.\n"
            "Samples have 3 types of values:\n"
            " - raw_value: raw ADC value from the detector (generally 12-bit, see detector info)\n"
            " - relative_value: value normalized to a 0-1 range\n"
            " - absolute_value: value normalized to a 0-1 range and corrected for the gain of the detector and emitter intensity\n"
            "\n"
            "Expected timeout can be estimated as:\n"
            "timeout_ms ≈ 2 × required_samples (in milliseconds).\n";
        info->addTag("Sensor module");
        auto example = FluorometerMeasurementDto::createShared();
        example->measurement_id = 1235;
        example->read = false;
        example->saturated = false;
        example->detector_gain = dto::GainEnum::x1;
        example->emitor_intensity = 0.5;
        example->timebase = dto::TimebaseEnum::logarithmic;
        example->timestamp = "2025-05-30T12:34:56";
        example->length_ms = 1000;
        example->required_samples = 1000;
        example->captured_samples = 998;
        example->missing_samples = 2;

        
        auto sample1 = FluorometerSampleDto::createShared();
        sample1->time_ms = 1.2f; 
        sample1->raw_value = 1023; 
        sample1->relative_value = 0.5f; 
        sample1->absolute_value = 0.125f; 

        auto sample2 = FluorometerSampleDto::createShared();
        sample2->time_ms = 10.0f;
        sample2->raw_value = 4095;
        sample2->relative_value = 1.0f;
        sample2->absolute_value = 0.95f;

        example->samples = {sample1, sample2};
        info->addResponse<Object<FluorometerMeasurementDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "OJIP data not available")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "OJIP data not available"}}));
    }

    ADD_CORS(retrieveLastFluorometerOjipData)
    ENDPOINT("GET", "/sensor/fluorometer/ojip/read_last", retrieveLastFluorometerOjipData);

    /**
     * @brief Retrieves information about the fluorometer detector.
     */
    ENDPOINT_INFO(getFluorometerDetectorInfo) {
        info->summary = "Retrieves information about the fluorometer detector";
        info->description = "Contains:\n"
                            "- detector peak wavelength in nm\n"
                            "- detector sensitivity\n"
                            "- sampling rate in kHz";
        info->addTag("Sensor module");
        auto example = FluorometerDetectorInfoDto::createShared();
        example->peak_wavelength = 750;
        example->sensitivity = 100;
        example->sampling_rate = 500;
        info->addResponse<Object<FluorometerDetectorInfoDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Fluorometer detector info not available")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Fluorometer detector info not available"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve detector info")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve detector info"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getFluorometerDetectorInfo)
    ENDPOINT("GET", "/sensor/fluorometer/detector/info", getFluorometerDetectorInfo);

    /**
     * @brief Retrieves the temperature of the fluorometer detector.
     */
    ENDPOINT_INFO(getFluorometerDetectorTemperature) {
        info->summary = "Retrieves temperature of the fluorometer detector";
        info->description = "Retrieves temperature of the fluorometer detector in °C. "
                        "High temperature (>40°C) can have negative effect on detector sensitivity.";
        info->addTag("Sensor module");
        auto example = TempDto::createShared();
        example->temperature = 25.5; 
        info->addResponse<Object<TempDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Fluorometer detector temperature not available")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Fluorometer detector temperature not available"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve temperature")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve temperature"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getFluorometerDetectorTemperature)
    ENDPOINT("GET", "/sensor/fluorometer/detector/temperature", getFluorometerDetectorTemperature);

    /**
     * @brief Retrieves information about the fluorometer emitor.
     */
    ENDPOINT_INFO(getFluorometerEmitorInfo) {
        info->summary = "Retrieves information about the fluorometer emitor";
        info->description = "Contains:\n"
                            "- peak wavelength in nm\n"
                            "- max power output in mW";
        info->addTag("Sensor module");
        auto example = FluorometerEmitorInfoDto::createShared();
        example->peak_wavelength = 525;
        example->power_output = 10000;
        info->addResponse<Object<FluorometerEmitorInfoDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Fluorometer emitor info not available")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Fluorometer emitor info not available"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve emitor info")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve emitor info"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getFluorometerEmitorInfo)
    ENDPOINT("GET", "/sensor/fluorometer/emitor/info", getFluorometerEmitorInfo);

    /**
     * @brief Retrieves the temperature of the fluorometer emitor.
     */
    ENDPOINT_INFO(getFluorometerEmitorTemperature) {
        info->summary = "Retrieves temperature of the fluorometer emitor";
        info->description = "Retrieves temperature of the fluorometer emitor in °C. "
                        "High temperature (>60°C) can have negative effect on emitor stability. "
                        "Measurement performed in quick succession can lead to higher temperature.";
        info->addTag("Sensor module");
        auto example = TempDto::createShared();
        example->temperature = 35.0; 
        info->addResponse<Object<TempDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Fluorometer emitor temperature not available")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Fluorometer emitor temperature not available"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve temperature")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve temperature"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getFluorometerEmitorTemperature)
    ENDPOINT("GET", "/sensor/fluorometer/emitor/temperature", getFluorometerEmitorTemperature);

    /**
     * @brief Request self-calibration of fluorometer.
     */
    ENDPOINT_INFO(calibrateFluorometer) {
        info->summary = "Calibrate fluorometer";
        info->description = 
            "Request self-calibration of fluorometer.\n"
            "For calibration cuvette should be empty or filled with clean medium.\n"
            "No algae or other particles should be present in cuvette.\n"
            "Calibration is preserved in persistent memory of module.\n"
            "And will be loaded during next power up.\n"
            "Body of request must be empty, but it is required to be present for future calibration extensions.";
        info->addTag("Sensor module");
        
        auto exampleRequest = SpectroCalibrateDto::createShared();
        exampleRequest->calibrationMode=nullptr;
        
        info->addConsumes<Object<SpectroCalibrateDto>>("application/json")
            .addExample("application/json", exampleRequest);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Calibration started")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Calibration started"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to start calibration")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to start calibration"}}));

    }
    ADD_CORS(calibrateFluorometer)
    ENDPOINT("POST", "/sensor/fluorometer/calibrate", calibrateFluorometer, BODY_STRING(String, requestBody));

    /**
     * @brief Reads number of channels available on spectrophotometer.
     */
    ENDPOINT_INFO(getSpectrophotometerChannels) {
        info->summary = "Reads number of channels available on spectrophotometer";
        info->description = 
            "Reads number of channels available on spectrophotometer.\n"
            "Each channel can be used to measure absorbance of different wavelength.\n"
            "Endpoints return N channels which are numbered from 0 to (N-1).";
        info->addTag("Sensor module");
        auto example = SpectroChannelsDto::createShared();
        example->channels = 6; 
        info->addResponse<Object<SpectroChannelsDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Spectrophotometer channels count not available")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Spectrophotometer channels count not available"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve channels count")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve channels count"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getSpectrophotometerChannels)
    ENDPOINT("GET", "/sensor/spectrophotometer/channels", getSpectrophotometerChannels);

    /**
     * @brief Read information about spectrophotometer channel.
     */
    ENDPOINT_INFO(getSpectrophotometerChannelInfo) {
        info->summary = "Read information about spectrophotometer channel";
        info->description = 
            "Read information about spectrophotometer channel.\n"
            "Contains:\n"
            "  - Peak wavelength in nm\n"
            "  - Half intensity peak width in nm";
        info->addTag("Sensor module");
        
        auto example = SpectroChannelInfoDto::createShared();
        example->channel = 1;
        example->peak_wavelength = 480;
        example->half_intensity_peak_width = 10;
        
        info->addResponse<Object<SpectroChannelInfoDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Channel not found or not available")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Channel not found or not available"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve channel info")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve channel info"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getSpectrophotometerChannelInfo)
    ENDPOINT("GET", "/sensor/spectrophotometer/channel_info/{channel}", getSpectrophotometerChannelInfo, PATH(oatpp::UInt8, channel));

    /**
     * @brief Measure all channels at once and return responses.
     */
    ENDPOINT_INFO(measureAllSpectrophotometerChannels) {
        info->summary = "Measure all channels at once and return responses";
        info->description = 
            "Measure all channels at once and return responses.\n"
            "Each channel can be used to measure absorbance of different wavelength.\n"
            "Endpoints return N channel which are numbered from 0 to (N-1).";
        info->addTag("Sensor module");
        
        auto example = SpectroMeasurementsDto::createShared();
        example->samples = oatpp::Vector<oatpp::Object<SingleChannelMeasurementDto>>::createShared();

        auto addExampleMeasurement = [&](int8_t channel, float relative, uint16_t absolute) {
            auto measurement = SingleChannelMeasurementDto::createShared();
            measurement->channel = channel;
            measurement->relative_value = relative;
            measurement->absolute_value = absolute;
            example->samples->push_back(measurement);
        };

        addExampleMeasurement(0, 0.125f, 28);
        addExampleMeasurement(1, 0.236f, 2015);
        addExampleMeasurement(2, 0.180f, 38);
        addExampleMeasurement(3, 0.452f, 1115);
        addExampleMeasurement(4, 0.561f, 3845);
        addExampleMeasurement(5, 0.473f, 5724);
        
        info->addResponse<Object<SpectroMeasurementsDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Spectrophotometer not available")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Spectrophotometer not available"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to perform measurements")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to perform measurements"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(measureAllSpectrophotometerChannels)
    ENDPOINT("POST", "/sensor/spectrophotometer/measure_all", measureAllSpectrophotometerChannels);

    /**
     * @brief Measure selected single channel and return response.
     */
    ENDPOINT_INFO(measureSingleSpectrophotometerChannel) {
        info->summary = "Measure selected single channel and return response";
        info->description = 
            "Measure selected single channel and return response.\n"
            "Each channel can be used to measure absorbance of different wavelength.\n"
            "To get information about wavelength channel_info/{channel} could be used.";
        info->addTag("Sensor module");
        
        auto example = SingleChannelMeasurementDto::createShared();
        example->channel = 1;
        example->relative_value = 0.236f;
        example->absolute_value = 5012;
        
        info->addResponse<Object<SingleChannelMeasurementDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Channel not available")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Channel not available"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to perform measurement")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to perform measurement"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(measureSingleSpectrophotometerChannel)
    ENDPOINT("POST", "/sensor/spectrophotometer/measure/{channel}", measureSingleSpectrophotometerChannel, PATH(Int8, channel));

    /**
     * @brief Retrieves the temperature of the spectrophotometer emitor.
     */
    ENDPOINT_INFO(getSpectrophotometerEmitorTemperature) {
        info->summary = "Read temperature of the spectrophotometer emitor";
        info->description = "Retrieves the temperature of the spectrophotometer emitor in °C.";
        info->addTag("Sensor module");
        auto example = TempDto::createShared();
        example->temperature = 30.2; 
        info->addResponse<Object<TempDto>>(Status::CODE_200, "application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Spectrophotometer emitor temperature not available")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Spectrophotometer emitor temperature not available"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve temperature")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve temperature"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getSpectrophotometerEmitorTemperature)
    ENDPOINT("GET", "/sensor/spectrophotometer/emitor/temperature", getSpectrophotometerEmitorTemperature);


    /**
     * @brief Request self-calibration of spectrophotometer.
     */
    ENDPOINT_INFO(calibrateSpectrophotometer) {
        info->summary = "Calibrate spectrophotometer";
        info->description = 
            "Request self-calibration of spectrophotometer.\n"
            "For calibration cuvette should be empty or filled with clean medium.\n"
            "No algae or other particles should be present in cuvette.\n"
            "Calibration is preserved in persistent memory of module.\n"
            "And will be loaded during next power up.\n"
            "Body of request must be empty, but it is required to be present for future calibration extensions.";
        info->addTag("Sensor module");
        
        auto exampleRequest = SpectroCalibrateDto::createShared();
        exampleRequest->calibrationMode=nullptr;
        
        info->addConsumes<Object<SpectroCalibrateDto>>("application/json")
            .addExample("application/json", exampleRequest);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Calibration started")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Calibration started"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to start calibration")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to start calibration"}}));

    }
    ADD_CORS(calibrateSpectrophotometer)
    ENDPOINT("POST", "/sensor/spectrophotometer/calibrate", calibrateSpectrophotometer, BODY_STRING(String, requestBody));

    /**
    * @brief Measures API response time without communication with RPI/CAN bus.
    */
   /*
    ENDPOINT_INFO(pingDirect) {
        info->summary = "Measure API response time. Used for testing.";
        info->addTag("Test");
        info->description = "Measures the time it takes for the API to respond without communication with RPI/CAN bus.";
        info->addResponse<String>(Status::CODE_200, "application/json");
    }   
    ADD_CORS(pingDirect)
    ENDPOINT("GET", "/ping-direct", pingDirect);
    */

// ==========================================
// Recipes
// ========================================== 

    /**
    * @brief Get list of recipes.
    */
    ENDPOINT_INFO(getRecipeList) {
        info->summary = "Get list of recipes";
        info->addTag("Recipes");
        info->description = "Return list of names of existing recipes.";
        info->addResponse<List<String>>(Status::CODE_200, "application/json", "List of recipe names");
    }
    ADD_CORS(getRecipeList)
    ENDPOINT("GET", "/recipes", getRecipeList);

    /**
    * @brief Get list of recipes.
    */
    ENDPOINT_INFO(reloadRecipeList) {
        info->summary = "Reload recipes from filesystem";
        info->addTag("Recipes");
        info->description = "Reloads recipes from the filesystem.";
        info->addResponse<List<String>>(Status::CODE_200, "application/json", "List of recipe names");
    }
    ADD_CORS(reloadRecipeList)
    ENDPOINT("PATCH", "/recipes", reloadRecipeList);

    /**
     * @brief Get recipe content
     */
    ENDPOINT_INFO(getRecipeContent) {
        info->summary = "Get recipe content";
        info->addTag("Recipes");
        info->description = "Return content of the recipe.";
        info->addResponse<String>(Status::CODE_200, "application/json", "Recipe content");
        info->addResponse<String>(Status::CODE_404, "application/json", "Recipe not found");
    }
    ADD_CORS(getRecipeContent)
    ENDPOINT("GET", "/recipes/{recipeName}", getRecipeContent, PATH(String, recipeName));

    /** 
     * @brief Updates Recipe content
     */
    ENDPOINT_INFO(updateRecipe) {
        info->summary = "Update recipe content";
        info->addTag("Recipes");
        info->description = "Update content of the recipe.";
        info->addConsumes<Object<ScriptDto>>("application/json");
        info->addResponse<String>(Status::CODE_200, "application/json", "Recipe updated successfully");
        info->addResponse<String>(Status::CODE_404, "application/json", "Recipe not found");
    }
    ADD_CORS(updateRecipe)
    ENDPOINT("PUT", "/recipes/{recipeName}", updateRecipe, PATH(String, recipeName), BODY_DTO(Object<ScriptDto>, body));
    
    /**
     * @brief Deletes recipe
     */
    ENDPOINT_INFO(deleteRecipe) {
        info->summary = "Delete recipe";
        info->addTag("Recipes");
        info->description = "Delete the recipe.";
        info->addResponse<String>(Status::CODE_200, "application/json", "Recipe deleted successfully");
        info->addResponse<String>(Status::CODE_404, "application/json", "Recipe not found");
    }
    ADD_CORS(deleteRecipe)
    ENDPOINT("DELETE", "/recipes/{recipeName}", deleteRecipe, PATH(String, recipeName));

// ==========================================
// Scheduler
// ==========================================   


    /**
     * @brief Retrieves the currently assigned recipe.
     */
    ENDPOINT_INFO(getRecipe) {
        info->summary = "Get scheduled recipe";
        info->addTag("Scheduler");
        info->description = "Retrieves the active (selected) recipe.";
        info->addResponse<Object<ScriptDto>>(Status::CODE_200, "application/json");
        info->addResponse<String>(Status::CODE_404, "application/json", "Recipe not found");
    }
    ADD_CORS(getRecipe)
    ENDPOINT("GET", "/scheduler/recipe", getRecipe);


    /**
     * @brief Select recipe to be run by the scheduler.
     */
    ENDPOINT_INFO(selectRecipe) {
        info->summary = "Assign recipe to scheduler";
        info->addTag("Scheduler");
        info->description = "Select the recipe to be run by the scheduler.";
        info->addResponse<String>(Status::CODE_200, "application/json", "Recipe selected successfully");
        info->addResponse<String>(Status::CODE_404, "application/json", "Recipe not found");
    }
    ADD_CORS(selectRecipe)
    ENDPOINT("POST", "/scheduler/recipe", selectRecipe, BODY_DTO(String, recipeName));



    /**
     * @brief Starts the scheduler.
     */
    ENDPOINT_INFO(startScheduler) {
        info->summary = "Start scheduler";
        info->addTag("Scheduler");
        info->description = "Starts the scheduler. The scheduler will run the uploaded script.";
        info->addResponse<Object<ScriptProcessIdDto>>(Status::CODE_200, "application/json");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to start scheduler.");
    }
    ADD_CORS(startScheduler)
    ENDPOINT("POST", "/scheduler/start", startScheduler);

    /**
     * @brief Stops the scheduler.
     */
    ENDPOINT_INFO(stopScheduler) {
        info->summary = "Stop scheduler";
        info->addTag("Scheduler");
        info->description = "Stops the scheduler. The scheduler will stop running the script.";
        info->addResponse<String>(Status::CODE_200, "application/json", "Scheduler stopped successfully.");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to stop scheduler.");
    }
    ADD_CORS(stopScheduler)
    ENDPOINT("POST", "/scheduler/stop", stopScheduler);

    /**
     * @brief Retrieves the runtime info of the scheduler.
     */
    ENDPOINT_INFO(getSchedulerInfo) {
        info->summary = "Get scheduler info";
        info->addTag("Scheduler");
        info->description = "Retrieves the runtime info of the scheduler.";
        info->addResponse<Object<ScriptRuntimeInfoDto>>(Status::CODE_200, "application/json");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to retrieve scheduler info");
    }
    ADD_CORS(getSchedulerInfo)
    ENDPOINT("GET", "/scheduler/runtime", getSchedulerInfo);

    ~SMBRController(); 
    static std::string moduleToString(Modules module);

private:
   template<typename TDto>
    std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> 
    readCanStat(const std::string& statName,
                const std::string& jsonField,
                std::function<oatpp::Object<TDto>(uint64_t)> makeDto) 
    {
        std::string path = "/sys/class/net/can0/statistics/" + statName;
        std::ifstream file(path);
        if (!file.is_open()) {
            auto errorDto = MessageDto::createShared();
            errorDto->message = "Failed to open CAN statistics file: " + path;
            return createDtoResponse(Status::CODE_500, errorDto);
        }

        uint64_t value = 0;
        file >> value;
        if (file.fail()) {
            auto errorDto = MessageDto::createShared();
            errorDto->message = "Failed to parse CAN statistics value from: " + path;
            return createDtoResponse(Status::CODE_500, errorDto);
        }

        auto dto = makeDto(value);
        return createDtoResponse(Status::CODE_200, dto);
    }
    
    uint64_t readCanValue(const std::string& statName);
    std::shared_ptr<ICommonModule> getModule(const oatpp::Enum<dto::ModuleEnum>::AsString& module);
    int getChannel(const dto::ChannelEnum& channel);
    Fluorometer_config::Gain getGain(const std::string& gainStr);
    Fluorometer_config::Timing getTimebase(const std::string& timebaseStr);

    std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> process(
        std::string name,
        std::function<std::shared_ptr<oatpp::web::protocol::http::outgoing::Response>()> body);

    std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> processBool(
        std::string name,
        std::function<bool()> body
    );

    std::string instanceToString(Instance instance);

    std::shared_ptr<std::promise<std::shared_ptr<oatpp::web::protocol::http::outgoing::Response>>> activeCapturePromise;
    std::thread captureWorker;
    std::condition_variable captureQueueCondition;
    std::atomic<bool> stopCaptureWorker = false;
    std::queue<std::function<void()>> captureQueue;
    static constexpr size_t MAX_QUEUE_SIZE = 4;
    std::mutex queueMutex;

    std::shared_ptr<std::shared_future<std::shared_ptr<oatpp::web::protocol::http::outgoing::Response>>> activeCaptureFuture;
    std::mutex activeCaptureMutex;
    std::mutex captureMutex;

    std::mutex retrieveMutex;
    std::condition_variable retrieveCondition;
    bool retrievingInProgress = false;
    std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> cachedResponse = nullptr;

private:
    std::shared_ptr<ISystemModule> systemModule;
    std::shared_ptr<IScheduler> scheduler_;
    std::shared_ptr<IRecipes> recipes_;
};

#include OATPP_CODEGEN_END(ApiController)
