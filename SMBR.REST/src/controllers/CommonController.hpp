#pragma once

#include "BaseController.hpp"

#include "dto/HwVersionDto.hpp"
#include "dto/LoadResponseDto.hpp"
#include "dto/MessageDto.hpp"
#include "dto/ModuleActionRequestDto.hpp"
#include "dto/ModuleEnum.hpp"
#include "dto/PingResponseDto.hpp"
#include "dto/TempDto.hpp"
#include "dto/VersionDto.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

#undef OATPP_MACRO_API_CONTROLLER_ADD_CORS_BODY_DEFAULT_METHODS
#define OATPP_MACRO_API_CONTROLLER_ADD_CORS_BODY_DEFAULT_METHODS "GET, POST, OPTIONS, PUT, PATCH, DELETE"

/**
 * @class CommonController
 * @brief Endpoints common to all modules, addressed by module name.
 */
class CommonController : public SMBRControllerBase {
public:
    CommonController(const std::shared_ptr<oatpp::web::mime::ContentMappers>& apiContentMappers,
                     std::shared_ptr<ISystemModule> systemModule);

    /**
     * @brief Sends a ping to the target module and returns the response time.
     */
    ENDPOINT_INFO(ping) {
        info->summary = "Send ping to target module";
        info->addTag("Common");
        info->description = 
            "Sends ping request to target module and wait for response. If response is not received in 1 seconds, then timeouts.\n\n"
            "On CAN bus first byte of message is Sequential_number, which is used to match request and response.";
        auto example = PingResponseDto::createShared();
        example->time_ms = 2.5f;
        info->addResponse<Object<PingResponseDto>>(Status::CODE_200, "application/json", "Ping received from module")
            .addExample("application/json", example);
        info->queryParams.add<oatpp::Int8>("instance").required = false;
        info->queryParams["instance"].description = "Pump module instance number (1-12). Required only when module=pump, ignored otherwise.";
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Missing or invalid instance for pump module")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Instance is required for pump module (1-12)"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Module not found")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Module not found"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Ping failed")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Ping failed"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
        }
    ADD_CORS(ping)
    ENDPOINT("GET", "/{module}/ping", ping,
        REQUEST(std::shared_ptr<IncomingRequest>, request), PATH(oatpp::Enum<dto::ModuleEnum>::AsString, module));

    /**
    * @brief Retrieves the CPU/MCU load of the specified module.
    */
    ENDPOINT_INFO(getCoreLoad) {
        info->summary = "Get module CPU/MCU load";
        info->addTag("Common");
        info->description = 
            "Gets the current workload values of the computing unit. This is the average utilization of all available cores." 
            "For example, for Rpi this is all available cores. For the RP2040, it depends on the module whether it uses only one or two cores." 
            "The number of available cores is part of the answer.";
        auto example = LoadResponseDto::createShared();
        example->load = 0.5f;
        info->addResponse<Object<LoadResponseDto>>(Status::CODE_200, "application/json", "Successfully retrieved load from module")
            .addExample("application/json", example);
        info->queryParams.add<oatpp::Int8>("instance").required = false;
        info->queryParams["instance"].description = "Pump module instance number (1-12). Required only when module=pump, ignored otherwise.";
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Missing or invalid instance for pump module")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Instance is required for pump module (1-12)"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Module not found")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Module not found"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve load")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve load"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getCoreLoad)
    ENDPOINT("GET", "/{module}/load", getCoreLoad,
        REQUEST(std::shared_ptr<IncomingRequest>, request), PATH(oatpp::Enum<dto::ModuleEnum>::AsString, module));
    
    /**
     * @brief Retrieves the CPU/MCU temperature of the specified module.
     */
    ENDPOINT_INFO(getCoreTemp) {
        info->summary = "Get module CPU/MCU temperature";
        info->addTag("Common");
        info->description = "Gets the current temperature of CPU/MCU core values of the computing unit.";
        auto example = TempDto::createShared();
        example->temperature = 30.2f;
        info->addResponse<Object<TempDto>>(Status::CODE_200, "application/json", "Successfully retrieved mcu temperature from module")
            .addExample("application/json", example);
        info->queryParams.add<oatpp::Int8>("instance").required = false;
        info->queryParams["instance"].description = "Pump module instance number (1-12). Required only when module=pump, ignored otherwise.";
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Missing or invalid instance for pump module")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Instance is required for pump module (1-12)"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Module not found")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Module not found"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve temperature")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve temperature"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getCoreTemp)
    ENDPOINT("GET", "/{module}/core_temp", getCoreTemp,
        REQUEST(std::shared_ptr<IncomingRequest>, request), PATH(oatpp::Enum<dto::ModuleEnum>::AsString, module));

    /**
     * @brief Retrieves the board temperature of the specified module.
     */
    ENDPOINT_INFO(getBoardTemp) {
        info->summary = "Get temperature of module board itself";
        info->addTag("Common");
        info->description = 
            "Gets the current temperature module board."
            "Sensor is in most cases placed on board around temperature intensive components or measuring equipment.";
        auto example = TempDto::createShared();
        example->temperature = 30.2f;
        info->addResponse<Object<TempDto>>(Status::CODE_200, "application/json", "Successfully retrieved board temperature from module")
            .addExample("application/json", example);
        info->queryParams.add<oatpp::Int8>("instance").required = false;
        info->queryParams["instance"].description = "Pump module instance number (1-12). Required only when module=pump, ignored otherwise.";
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Missing or invalid instance for pump module")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Instance is required for pump module (1-12)"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Module not found")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Module not found"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve temperature")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve temperature"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getBoardTemp)
    ENDPOINT("GET", "/{module}/board_temp", getBoardTemp,
        REQUEST(std::shared_ptr<IncomingRequest>, request), PATH(oatpp::Enum<dto::ModuleEnum>::AsString, module));

    /**
    * @brief Restarts the specified module into application mode.
    */
    ENDPOINT_INFO(postRestart) {
        info->summary = "Restart module into application mode";
        info->addTag("Common");
        info->description = 
            "This will reset the module and it will start main application firmware again.\n\n" 
            "UID of the module is required in order to confirm that correct module is selected by request.";
        auto example = ModuleActionRequestDto::createShared();
        example->uid = "0x0123456789ab";
        info->addConsumes<Object<ModuleActionRequestDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Successfully restarted module")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Successfully restarted module"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Module not found")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Module not found"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to restart module")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to restart module"}}));
    }
    ADD_CORS(postRestart)
    ENDPOINT("POST", "/{module}/restart", postRestart, PATH(oatpp::Enum<dto::ModuleEnum>::AsString, module), BODY_DTO(Object<ModuleActionRequestDto>, body));

    /**
    * @brief Reboots the specified module in USB bootloader mode.
    */
    ENDPOINT_INFO(postUsbBootloader) {
        info->summary = "Reboot module in built-in USB bootloader mode";
        info->addTag("Common");
        info->description = 
            "This will reset the module and put it into USB bootloader mode so new firmware can be flashed via USB-C connector on board.\n\n"
            "UID of the module is required in order to confirm that correct module is selected by request.";
        auto example = ModuleActionRequestDto::createShared();
        example->uid = "0x0123456789ab";
        info->addConsumes<Object<ModuleActionRequestDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Successfully restarted module in USB bootloader mode")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Successfully restarted module in USB bootloader mode"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Module not found")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Module not found"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to restart module")
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
        info->summary = "Reboot module in bootloader (katapult) mode";
        info->addTag("Common");
        info->description =
            "This will reset the module and put it into CAN bus bootloader mode so new firmware can be flashed over CAN bus from RPi.\n\n"
            "UID of the module is required in order to confirm that correct module is selected by request.";
        auto example = ModuleActionRequestDto::createShared();
        example->uid = "0x0123456789ab";
        info->addConsumes<Object<ModuleActionRequestDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Successfully restarted module in CAN bus bootloader mode")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Successfully restarted module in CAN bus bootloader mode"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Module not found")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Module not found"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to restart module")
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
        info->description = 
            "Gets the current firmware version flashed on the module.\n" 
            "This is the version of the main application firmware running on the module.\n" 
            "Version is in format X.Y.Z, where X is major version and Y is minor version.\n" 
            "Z is Patch version, which is incremented with commit above major.minor version.\n" 
            "Hash is the git commit hash on which was this version built. Hash is 7-16 characters long.\n" 
            "And is generaly formatted as {:07x}, this means that longer hashes then 7 character does not have leading zeroes.\n" 
            "Dirty flag is set to true if the firmware was built from a dirty git repository (uncommitted changes).\n" 
            "Different version of firmware are compatible. Core module version numbering can be vastly different from other modules, but still follows the same pattern.\n" 
            "This is because Core module is only virtual and running on SBC (Rpi).";
        auto example = VersionDto::createShared();
        example->version = "1.3.5";
        example->hash = "ca123fe";
        example->dirty = false;
        info->addResponse<Object<VersionDto>>(Status::CODE_200, "application/json", "Successfully retrieved firmware version from module")
            .addExample("application/json", example);
        info->queryParams.add<oatpp::Int8>("instance").required = false;
        info->queryParams["instance"].description = "Pump module instance number (1-12). Required only when module=pump, ignored otherwise.";
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Missing or invalid instance for pump module")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Instance is required for pump module (1-12)"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Module not found")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Module not found"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve firmware version")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve firmware version"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getFwVersion)
    ENDPOINT("GET", "/{module}/fw_version", getFwVersion,
        REQUEST(std::shared_ptr<IncomingRequest>, request), PATH(oatpp::Enum<dto::ModuleEnum>::AsString, module));

    /**
     * @brief Retrieves hardware version information.
     */
    ENDPOINT_INFO(getHwVersion) {
        info->summary = "Get module hardware version";
        info->addTag("Common");
        info->description = 
            "Gets the current hardware version of the module.\n"
            "This is the revision of hardware, read directly from pcb.\n"
            "Version is in format X.Y, where X is major version and Y is minor version.\n"
            "Different hardware versions are compatible with each other and are used in one device.";
        auto example = HwVersionDto::createShared();
        example->version = "1.2";
        info->addResponse<Object<HwVersionDto>>(Status::CODE_200, "application/json", "Successfully retrieved hardware version from module")
            .addExample("application/json", example);
        info->queryParams.add<oatpp::Int8>("instance").required = false;
        info->queryParams["instance"].description = "Pump module instance number (1-12). Required only when module=pump, ignored otherwise.";
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Missing or invalid instance for pump module")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Instance is required for pump module (1-12)"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Module not found")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Module not found"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve hardware version")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve hardware version"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getHwVersion)
    ENDPOINT("GET", "/{module}/hw_version", getHwVersion, REQUEST(std::shared_ptr<IncomingRequest>, request), PATH(oatpp::Enum<dto::ModuleEnum>::AsString, module));

};

#include OATPP_CODEGEN_END(ApiController)
