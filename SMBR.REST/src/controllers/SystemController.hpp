#pragma once

#include "BaseController.hpp"

#include "dto/CollisionsDto.hpp"
#include "dto/MessageDto.hpp"
#include "dto/ModuleEnum.hpp"
#include "dto/ModuleInfoDto.hpp"
#include "dto/ModuleIssueDto.hpp"
#include "dto/ModuleIssuesListDto.hpp"
#include "dto/RxDroppedDto.hpp"
#include "dto/RxErrorsDto.hpp"
#include "dto/RxPacketsDto.hpp"
#include "dto/SystemProblemDto.hpp"
#include "dto/SystemProblemResponseDto.hpp"
#include "dto/TxDroppedDto.hpp"
#include "dto/TxErrorsDto.hpp"
#include "dto/TxPacketsDto.hpp"

#include <fstream>
#include <string>

#include OATPP_CODEGEN_BEGIN(ApiController)

#undef OATPP_MACRO_API_CONTROLLER_ADD_CORS_BODY_DEFAULT_METHODS
#define OATPP_MACRO_API_CONTROLLER_ADD_CORS_BODY_DEFAULT_METHODS "GET, POST, OPTIONS, PUT, PATCH, DELETE"

/**
 * @class SystemController
 * @brief System endpoints of the SMBR API.
 */
class SystemController : public SMBRControllerBase {
public:
    SystemController(const std::shared_ptr<oatpp::web::mime::ContentMappers>& apiContentMappers,
                     std::shared_ptr<ISystemModule> systemModule);

    /**
     * @brief Retrieves available modules and their unique CAN IDs.
     */
    ENDPOINT_INFO(getSystemModules) {
        info->summary = "Determines which all modules are available on the device and their respective unique CAN IDs";
        info->addTag("System");
        info->description = "Returns a list of all modules that have responded to the identification message and can therefore be considered available on the device.\n\n"
        "Included are the unique CAN IDs of the modules. Unique ID is 6-byte identifier which is made from:\n"
        "  - MCU unique ID from Flash memory hashed by fast-hash ans same as Katapult bootloader id\n"
        "  - RPi unique ID is lower part of serial number `/sys/firmware/devicetree/base/serial-number`";
        auto example = List<Object<ModuleInfoDto>>::createShared();
        auto module = ModuleInfoDto::createShared();
        module->module_type = "sensor";
        module->uid = "0x0123456789ab";
        module->instance = "Exclusive"; 
        example->push_back(module);
        info->addResponse<List<Object<ModuleInfoDto>>>(Status::CODE_200, "application/json", "List of available devices")
            .addExample("application/json", example);
    }
    ADD_CORS(getSystemModules)
    ENDPOINT("GET", "/system/modules", getSystemModules);

    /**
     * @brief Lists all detected system errors or confirms that the system is operating normally.
     */
    ENDPOINT_INFO(getSystemErrors) {
    info->summary = "Lists all detected system errors or confirms that the system is operating normally";
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
        info->summary = "Lists all detected system warnings or confirms that the system is operating normally";
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
        info->summary = "Lists all active module issues or confirms that the system is operating normally";
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
        exampleOk->message = "No active module issues detected. System is operating normally.";
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
        info->addResponse<Object<RxPacketsDto>>(Status::CODE_200, "application/json", "Number of received CAN packets")
            .addExample("application/json", successExample);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve CAN data")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve CAN data"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
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
        info->addResponse<Object<TxPacketsDto>>(Status::CODE_200, "application/json", "Number of transmitted CAN packets")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve CAN data")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve CAN data"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
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
        info->addResponse<Object<RxErrorsDto>>(Status::CODE_200, "application/json", "Number of receive errors")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve CAN data")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve CAN data"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
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
        info->addResponse<Object<TxErrorsDto>>(Status::CODE_200, "application/json", "Number of transmit errors")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve CAN data")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve CAN data"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
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
        info->addResponse<Object<RxDroppedDto>>(Status::CODE_200, "application/json", "Number of dropped received packets")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve CAN data")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve CAN data"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
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
        info->addResponse<Object<TxDroppedDto>>(Status::CODE_200, "application/json", "Number of dropped transmitted packets")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve CAN data")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve CAN data"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
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
        info->addResponse<Object<CollisionsDto>>(Status::CODE_200, "application/json", "Number of collisions")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve CAN data")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve CAN data"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getCanCollisions)
    ENDPOINT("GET", "/system/can/collisions", getCanCollisions);

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

};

#include OATPP_CODEGEN_END(ApiController)
