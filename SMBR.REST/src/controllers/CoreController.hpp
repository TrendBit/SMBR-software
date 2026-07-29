#pragma once

#include "BaseController.hpp"

#include "dto/CurrentDto.hpp"
#include "dto/HostnameDto.hpp"
#include "dto/HostnameRequestDto.hpp"
#include "dto/IpDto.hpp"
#include "dto/MessageDto.hpp"
#include "dto/ModelDto.hpp"
#include "dto/PowerDrawDto.hpp"
#include "dto/SIDDto.hpp"
#include "dto/SerialDto.hpp"
#include "dto/SupplyTypeDto.hpp"
#include "dto/VoltageDto.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

#undef OATPP_MACRO_API_CONTROLLER_ADD_CORS_BODY_DEFAULT_METHODS
#define OATPP_MACRO_API_CONTROLLER_ADD_CORS_BODY_DEFAULT_METHODS "GET, POST, OPTIONS, PUT, PATCH, DELETE"

/**
 * @class CoreController
 * @brief Endpoints of the core module (SBC).
 */
class CoreController : public SMBRControllerBase {
public:
    CoreController(const std::shared_ptr<oatpp::web::mime::ContentMappers>& apiContentMappers,
                   std::shared_ptr<ISystemModule> systemModule);

    /**
     * @brief Retrieves the short ID (SID) of the device.
     */
    ENDPOINT_INFO(getShortID) {
        info->summary = "Get Short ID (SID) of the device";
        info->addTag("Core module");
        info->description = "Gets short ID number (SID) of the device, 4 hexadecimal characters. Is possible that this ID is not unique across all devices.";
        auto example = SIDDto::createShared();
        example->sid = "c0de";
        info->addResponse<Object<SIDDto>>(Status::CODE_200, "application/json", "Successfully retrieved SID of the device")
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
        info->description = 
            "Gets IP address of the device. This is the IP address of the device on the local network." 
            "If IP address was not assigned yet, then will return 0.0.0.0";
        auto example = IpDto::createShared();
        example->ipAddress = "192.168.1.100";
        info->addResponse<Object<IpDto>>(Status::CODE_200, "application/json", "Successfully retrieved IP address of the device")
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
        info->summary = "Get hostname of the device (8-chars)";
        info->addTag("Core module");
        info->description = 
            "Gets hostname of the device. This is the hostname of the device on the local network." 
            "Hostname is truncated to only 8 characters, so it can be transferred over CAN bus in one message.";
        auto example = HostnameDto::createShared();
        example->hostname = "smpbr_01";
        info->addResponse<Object<HostnameDto>>(Status::CODE_200, "application/json", "Successfully retrieved hostname of the device")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve hostname")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve hostname"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getHostname)
    ENDPOINT("GET", "/core/hostname", getHostname);

    /**
     * @brief Sets the hostname of the device.
     */
    ENDPOINT_INFO(setHostname) {
        info->summary = "Set hostname of the device";
        info->addTag("Core module");
        info->description =
            "Sets the hostname of the device by writing it to `/data/etc/hostname`.\n\n"
            "Hostname is limited to 8 characters (letters, digits, hyphens and underscores).\n\n"
            "**Caution:** the device automatically reboots at the end of every call to this endpoint "
            "so that the new hostname takes effect.";
        auto example = HostnameRequestDto::createShared();
        example->hostname = "smpbr01";
        info->addConsumes<Object<HostnameRequestDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Hostname set, device is rebooting")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Hostname set to 'smpbr01'. Device is rebooting for the change to take effect."}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Invalid hostname")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Invalid request body: hostname must be 1-8 characters long and contain only letters, digits, hyphens and underscores"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to set hostname or trigger reboot")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to set hostname: Failed to open /data/etc/hostname for writing"}}));
    }
    ADD_CORS(setHostname)
    ENDPOINT("POST", "/core/hostname", setHostname, BODY_DTO(Object<HostnameRequestDto>, body));

    /**
     * @brief Retrieves the serial number of the device.
     */
    ENDPOINT_INFO(getSerialNumber) {
        info->summary = "Get serial number of the device";
        info->addTag("Core module");
        info->description = "Gets serial number of the device. Serial number is unique for each device. Serial number of device is RPi serial number.";
        auto example = SerialDto::createShared();
        example->serial = 1907977600;
        info->addResponse<Object<SerialDto>>(Status::CODE_200, "application/json", "Successfully retrieved serial number of the device")
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
        info->addResponse<Object<ModelDto>>(Status::CODE_200, "application/json", "Successfully retrieved model of the device")
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
        info->description = 
            "Gets type of power supply which powers the device.\n\n" 
            "Three possible options: \n\n"
            "- VIN: external power supply adapter\n"
            "- PoE: Power over Ethernet from RJ45 on RPi (15W)\n"
            "- PoE_HB: Variant of PoE with higher power budget (25-30W)\n\n"
            "If POE_HB if connected then always PoE is also connected.";
        auto example = SupplyTypeDto::createShared();
        example->vin = true;
        example->poe = false;
        example->poe_hb = false;
        info->addResponse<Object<SupplyTypeDto>>(Status::CODE_200, "application/json", "Successfully retrieved type of power supply")
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
        info->summary = "Get voltage of 5V power rail on device";
        info->addTag("Core module");
        info->description = 
            "Gets voltage of 5V power rail on device in volts. This rail powers RPi and all connected modules." 
            "Voltage rail 3.3V is derived on each module separately from this rail.";
        auto example = VoltageDto::createShared();
        example->voltage = 5.035f;
        info->addResponse<Object<VoltageDto>>(Status::CODE_200, "application/json", "Successfully retrieved voltage at 5V power rail")
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
        info->summary = "Get voltage at VIN power rail (12V) on device";
        info->addTag("Core module");
        info->description = 
            "Gets voltage of VIN power rail on device in volts." 
            "This voltage is supplied by external power supply adapter. Nominal value is 12V, but can be in range 11-13V.";
        auto example = VoltageDto::createShared();
        example->voltage = 11.995f;
        info->addResponse<Object<VoltageDto>>(Status::CODE_200, "application/json", "Successfully retrieved voltage at VIN power rail")
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
        info->summary = "Get voltage at PoE power rail (12V) on device";
        info->addTag("Core module");
        info->description = 
            "Gets voltage of PoE power rail on device in volts." 
            "This voltage is supplied by Power over Ethernet (PoE) from RJ45 on RPi." 
            "Nominal value is 12V, but can be in range 10-12.5 V. If PoE is not connected then this value should be close to 0.";
        auto example = VoltageDto::createShared();
        example->voltage = 12.01f;
        info->addResponse<Object<VoltageDto>>(Status::CODE_200, "application/json", "Successfully retrieved voltage at PoE power rail")
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
        info->summary = "Get current consumption of the device";
        info->addTag("Core module");
        info->description = 
            "Gets current consumption of the device in amperes." 
            "This is the current consumption of the whole device, including RPi and all connected modules." 
            "Current should be in range 0-5 A. When idle should be around 0.2-0.5 A.";
        auto example = CurrentDto::createShared();
        example->current = 0.9f;
        info->addResponse<Object<CurrentDto>>(Status::CODE_200, "application/json", "Successfully retrieved current consumption of the device")
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
        info->summary = "Get power draw of the device";
        info->addTag("Core module");
        info->description = 
            "Gets power draw of the device in watts." 
            "This is the power draw of the whole device, including RPi and all connected modules." 
            "Power draw is calculated from voltage and current consumption. Normal range is around 2-30W, but with external adapter can go up to 60W.";
        auto example = PowerDrawDto::createShared();
        example->power_draw = 4.2f;
        info->addResponse<Object<PowerDrawDto>>(Status::CODE_200, "application/json", "Successfully retrieved power draw of the device")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve power draw")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve power draw"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getPowerDraw)
    ENDPOINT("GET", "/core/supply/power_draw", getPowerDraw);

};

#include OATPP_CODEGEN_END(ApiController)
