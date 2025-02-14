#pragma once

#include "oatpp/macro/component.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "dto/ModuleEnum.hpp"
#include "dto/ChannelEnum.hpp"
#include "dto/MyPingResponseDto.hpp"
#include "dto/MyTempDto.hpp"
#include "dto/MyModuleInfoDto.hpp"
#include "dto/MyLoadResponseDto.hpp"
#include "dto/MyModuleActionRequestDto.hpp"
#include "dto/MyIntensitiesDto.hpp"
#include "dto/MyIntensityDto.hpp"
#include "dto/MySpeedDto.hpp"
#include "dto/MyFlowrateDto.hpp"
#include "dto/MyMoveDto.hpp"
#include "dto/MyRpmDto.hpp"
#include "dto/MyStirDto.hpp"
#include "dto/MySIDDto.hpp"
#include "dto/MyIpDto.hpp"
#include "dto/MyHostnameDto.hpp"
#include "dto/MySerialDto.hpp"
#include "dto/MySupplyTypeDto.hpp"
#include "dto/MyVoltageDto.hpp"
#include "dto/MyCurrentDto.hpp"
#include "dto/MyPowerDrawDto.hpp"
#include "dto/MyScriptDto.hpp"
#include "dto/MyScriptProcessIdDto.hpp"
#include "dto/MyScriptRuntimeInfoDto.hpp"

#include "oatpp/data/mapping/ObjectMapper.hpp"

#include <future>
#include <iomanip>
#include <cstdint>
#include <utility>
#include <unordered_set>
#include <optional>

#include <thread> 
#include <chrono> 

#include <iostream>

#include "SMBR/ISystemModule.hpp"
#include "SMBR/Scheduler.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

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
        info->addResponse<List<Object<MyModuleInfoDto>>>(Status::CODE_200, "application/json");
        info->addResponse<String>(Status::CODE_504, "application/json");
    }
    ADD_CORS(getSystemModules)
    ENDPOINT("GET", "/system/modules", getSystemModules);

    /**
     * @brief Sends a ping to the target module and returns the response time.
     */
    ENDPOINT_INFO(ping) {
        info->summary = "Send ping to target module";
        info->addTag("Common");
        info->description = "Sends ping request to target module and waits for response.";
        info->addResponse<Object<MyPingResponseDto>>(Status::CODE_200, "application/json");
        info->addResponse<String>(Status::CODE_500, "application/json");
        info->addResponse<String>(Status::CODE_404, "application/json");
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
        info->addResponse<Object<MyLoadResponseDto>>(Status::CODE_200, "application/json");
        info->addResponse<String>(Status::CODE_404, "application/json", "Module not found");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to retrieve load");
        info->addResponse<String>(Status::CODE_504, "application/json", "Request timed out");
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
        info->addResponse<Object<MyTempDto>>(Status::CODE_200, "application/json");
        info->addResponse<String>(Status::CODE_404, "application/json", "Module not found");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to retrieve temperature");
        info->addResponse<String>(Status::CODE_504, "application/json", "Request timed out");
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
    info->addResponse<Object<MyTempDto>>(Status::CODE_200, "application/json");
    info->addResponse<String>(Status::CODE_404, "application/json", "Module not found");
    info->addResponse<String>(Status::CODE_500, "application/json", "Failed to retrieve temperature");
    info->addResponse<String>(Status::CODE_504, "application/json", "Request timed out");
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
        info->addConsumes<Object<MyModuleActionRequestDto>>("application/json");
        info->addResponse<String>(Status::CODE_200, "application/json", "Successfully restarted module");
        info->addResponse<String>(Status::CODE_404, "application/json", "Module not found");
    }
    ADD_CORS(postRestart)
    ENDPOINT("POST", "/{module}/restart", postRestart, PATH(oatpp::Enum<dto::ModuleEnum>::AsString, module), BODY_DTO(Object<MyModuleActionRequestDto>, body));

    /**
    * @brief Reboots the specified module in USB bootloader mode.
    */
    ENDPOINT_INFO(postUsbBootloader) {
        info->summary = "Reboot module in USB bootloader mode";
        info->addTag("Common");
        info->description = 
            "This will reset the module and put it into USB bootloader mode so new firmware can be flashed via USB-C connector on board. "
            "UID of the module is required in order to confirm that correct module is selected by request.";
        info->addConsumes<Object<MyModuleActionRequestDto>>("application/json");
        info->addResponse<String>(Status::CODE_200, "application/json", "Successfully restarted module in usb bootloader mode");
        info->addResponse<String>(Status::CODE_404, "application/json", "Module not found");
    }
    ADD_CORS(postUsbBootloader)
    ENDPOINT("POST", "/{module}/usb_bootloader", postUsbBootloader, 
         PATH(oatpp::Enum<dto::ModuleEnum>::AsString, module), 
         BODY_DTO(Object<MyModuleActionRequestDto>, body));  

    /**
    * @brief Reboots the specified module in CAN bootloader mode.
    */
    ENDPOINT_INFO(postCanBootloader) {
        info->summary = "Reboot module in CAN bootloader mode";
        info->addTag("Common");
        info->description =
            "This will reset the module and put it into CAN bootloader mode so new firmware can be flashed over CAN bus from RPi. "
            "UID of the module is required in order to confirm that correct module is selected by request.";
        info->addConsumes<Object<MyModuleActionRequestDto>>("application/json");
        info->addResponse<String>(Status::CODE_200, "application/json", "Successfully restarted module in CAN bootloader mode");
        info->addResponse<String>(Status::CODE_404, "application/json", "Module not found");
    }
    ADD_CORS(postCanBootloader)
    ENDPOINT("POST", "/{module}/can_bootloader", postCanBootloader,
         PATH(oatpp::Enum<dto::ModuleEnum>::AsString, module),
         BODY_DTO(Object<MyModuleActionRequestDto>, body));

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
        info->addResponse<Object<MySIDDto>>(Status::CODE_200, "application/json");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to retrieve SID");
        info->addResponse<String>(Status::CODE_504, "application/json", "Request timed out");
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
        info->addResponse<Object<MyIpDto>>(Status::CODE_200, "application/json");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to retrieve IP address");
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
        info->addResponse<Object<MyHostnameDto>>(Status::CODE_200, "application/json");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to retrieve hostname");
        info->addResponse<String>(Status::CODE_504, "application/json", "Request timed out");
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
        info->addResponse<Object<MySerialDto>>(Status::CODE_200, "application/json");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to retrieve serial number");
        info->addResponse<String>(Status::CODE_504, "application/json", "Request timed out");
    }
    ADD_CORS(getSerialNumber)
    ENDPOINT("GET", "/core/serial", getSerialNumber);

    /**
     * @brief Retrieves the type of power supply powering the device.
     */
    ENDPOINT_INFO(getPowerSupplyType) {
        info->summary = "Get type of power supply";
        info->addTag("Core module");
        info->description = "Retrieves the type of power supply powering the device. The options are: - VIN: external power supply adapter. - PoE: Power over Ethernet from RJ45 on RPi (15W). - PoE_HB: Variant of PoE with higher power budget (25-30W).";
        info->addResponse<Object<MySupplyTypeDto>>(Status::CODE_200, "application/json");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to retrieve power supply type");
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
        info->addResponse<Object<MyVoltageDto>>(Status::CODE_200, "application/json");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to retrieve voltage");
        info->addResponse<String>(Status::CODE_504, "application/json", "Request timed out");
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
        info->addResponse<Object<MyVoltageDto>>(Status::CODE_200, "application/json");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to retrieve VIN voltage");
        info->addResponse<String>(Status::CODE_504, "application/json", "Request timed out");
    }
    ADD_CORS(getVoltageVIN)
    ENDPOINT("GET", "/core/supply/vin", getVoltageVIN);

    /**
     * @brief Retrieves the voltage of PoE power rail (12V).
     */
    ENDPOINT_INFO(getPoEVoltage) {
    info->summary = "Get Voltage at PoE Power Rail (12V)";
    info->addTag("Core module");
    info->description = "Retrieves the voltage of the PoE power rail, supplied by Power over Ethernet (PoE) from RJ45 on RPi.";
    info->addResponse<Object<MyVoltageDto>>(Status::CODE_200, "application/json");
    info->addResponse<String>(Status::CODE_504, "application/json", "Request timed out");
    info->addResponse<String>(Status::CODE_500, "application/json", "Internal error or invalid data");
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
    info->addResponse<Object<MyCurrentDto>>(Status::CODE_200, "application/json");
    info->addResponse<String>(Status::CODE_504, "application/json", "Request timed out");
    info->addResponse<String>(Status::CODE_500, "application/json", "Internal error or invalid data");
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
    info->addResponse<Object<MyPowerDrawDto>>(Status::CODE_200, "application/json");
    info->addResponse<String>(Status::CODE_504, "application/json", "Request timed out");
    info->addResponse<String>(Status::CODE_500, "application/json", "Internal error or invalid data");
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
        info->addConsumes<Object<MyIntensitiesDto>>("application/json");
        info->addResponse<String>(Status::CODE_200, "application/json", "Intensity set successfully.");
        info->addResponse<String>(Status::CODE_400, "application/json", "Invalid intensity value.");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to set intensity.");
    }
    ADD_CORS(setIntensities)
    ENDPOINT("POST", "/control/led_intensity", setIntensities, BODY_DTO(Object<MyIntensitiesDto>, body));      

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
        info->addConsumes<Object<MyIntensityDto>>("application/json");
        info->addResponse<String>(Status::CODE_200, "application/json", "Intensity set successfully.");
        info->addResponse<String>(Status::CODE_400, "application/json", "Invalid intensity or channel value.");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to set intensity.");
    }
    ADD_CORS(setIntensity)
    ENDPOINT("POST", "/control/led_intensity/{channel}", setIntensity, PATH(oatpp::Enum<dto::ChannelEnum>::AsString, channel), BODY_DTO(Object<MyIntensityDto>, body));

    /**
    * @brief Retrieves the current intensity of the selected LED channel.
    */
    ENDPOINT_INFO(getIntensity) {
        info->summary = "Retrieves current intensity of selected channel of LED panel";
        info->description = "Retrieves current intensity of selected channel of LED panel.";
        info->addTag("Control module");
        info->addResponse<Object<MyIntensityDto>>(Status::CODE_200, "application/json");
        info->addResponse<String>(Status::CODE_404, "application/json", "Channel not found");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to retrieve intensity");
        info->addResponse<String>(Status::CODE_504, "application/json", "Request timed out");
    }
    ADD_CORS(getIntensity)
    ENDPOINT("GET", "/control/led_intensity/{channel}", getIntensity, PATH(oatpp::Enum<dto::ChannelEnum>, channel));

    /**
    * @brief Retrieves the temperature of the LED panel.
    */
    ENDPOINT_INFO(getLedTemperature) {
        info->summary = "Get LED panel temperature";
        info->addTag("Control module");
        info->description = "Retrieves the current temperature of the LED panel in °C.";
        info->addResponse<Object<MyTempDto>>(Status::CODE_200, "application/json");
        info->addResponse<String>(Status::CODE_404, "application/json", "LED panel not available");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to retrieve LED temperature");
        info->addResponse<String>(Status::CODE_504, "application/json", "Request timed out");
    }
    ADD_CORS(getLedTemperature)
    ENDPOINT("GET", "/control/led_temperature", getLedTemperature);

    /**
     * @brief Sets the intensity of heating or cooling.
     */
    ENDPOINT_INFO(setHeaterIntensity) {
        info->summary = "Set heater intensity";
        info->description = "Sets the intensity of heating or cooling in range -1.0 (cooling) to 1.0 (heating). "
                            "This value can be overwritten by the regulation algorithm if temperature regulation (target temperature) is set.";
        info->addTag("Control module");
        info->addConsumes<Object<MyIntensityDto>>("application/json");
        info->addResponse<String>(Status::CODE_200, "application/json", "Intensity set successfully.");
        info->addResponse<String>(Status::CODE_400, "application/json", "Invalid intensity value.");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to set intensity.");
    }
    ADD_CORS(setHeaterIntensity)
    ENDPOINT("POST", "/control/heater/intensity", setHeaterIntensity, BODY_DTO(Object<MyIntensityDto>, body));

    /**
     * @brief Retrieves the current intensity of heating or cooling.
     */
    ENDPOINT_INFO(getHeaterIntensity) {
        info->summary = "Get heater intensity";
        info->addTag("Control module");
        info->description = "Retrieves current intensity of heating or cooling. Range is -1.0 (cooling) to 1.0 (heating). Intensity can be modified by regulation algorithm if temperature regulation (target temperature) is set.";
        info->addResponse<Object<MyIntensityDto>>(Status::CODE_200, "application/json");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to retrieve heater intensity");
        info->addResponse<String>(Status::CODE_504, "application/json", "Request timed out");
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
        info->addConsumes<Object<MyTempDto>>("application/json");
        info->addResponse<String>(Status::CODE_200, "application/json", "Target temperature set successfully.");
        info->addResponse<String>(Status::CODE_400, "application/json", "Invalid target temperature.");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to set target temperature.");
    }
    ADD_CORS(setHeaterTargetTemperature)
    ENDPOINT("POST", "/control/heater/target_temperature", setHeaterTargetTemperature, BODY_DTO(Object<MyTempDto>, body));

    /**
     * @brief Retrieves the currently set target temperature for the heater.
     */
    ENDPOINT_INFO(getHeaterTargetTemperature) {
        info->summary = "Get heater target temperature";
        info->addTag("Control module");
        info->description = "Retrieves the currently set target temperature for the heater (temperature of bottle) in °C.";
        info->addResponse<Object<MyTempDto>>(Status::CODE_200, "application/json");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to retrieve heater target temperature");
        info->addResponse<String>(Status::CODE_504, "application/json", "Request timed out");
    }
    ADD_CORS(getHeaterTargetTemperature)
    ENDPOINT("GET", "/control/heater/target_temperature", getHeaterTargetTemperature);

    /**
     * @brief Retrieves the current temperature of the heater plate (metal heatspreader).
     */
    ENDPOINT_INFO(getHeaterPlateTemperature) {
        info->summary = "Get heater plate temperature";
        info->addTag("Control module");
        info->description = "Retrieves temperature of plate (metal heatspreader) which is controlling temperature of bottle. Sensor is thermistor connected from back side of heater plate.";
        info->addResponse<Object<MyTempDto>>(Status::CODE_200, "application/json");
        info->addResponse<String>(Status::CODE_404, "application/json", "Heater plate not available");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to retrieve heater plate temperature");
        info->addResponse<String>(Status::CODE_504, "application/json", "Request timed out");
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
        info->addResponse<String>(Status::CODE_200, "application/json", "Heater was turned off.");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to turn off heater.");
    }
    ADD_CORS(turnOffHeater)
    ENDPOINT("POST", "/control/heater/turn_off", turnOffHeater);

    /**
     * @brief Sets the speed of the cuvette pump.
     */
    ENDPOINT_INFO(setCuvettePumpSpeed) {
        info->summary = "Set cuvette pump speed";
        info->description = "Sets the speed of the cuvette pump in range -1.0 (out) to 1.0 (in).";
        info->addTag("Control module");
        info->addConsumes<Object<MySpeedDto>>("application/json");
        info->addResponse<String>(Status::CODE_200, "application/json", "Speed set successfully.");
        info->addResponse<String>(Status::CODE_400, "application/json", "Invalid speed value.");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to set pump speed.");
    }
    ADD_CORS(setCuvettePumpSpeed)
    ENDPOINT("POST", "/control/cuvette_pump/speed", setCuvettePumpSpeed, BODY_DTO(Object<MySpeedDto>, body));

    /**
     * @brief Retrieves the current speed of the cuvette pump.
     */
    ENDPOINT_INFO(getCuvettePumpSpeed) {
        info->summary = "Get cuvette pump speed";
        info->addTag("Control module");
        info->description = "Retrieves current speed of the cuvette pump. Range is -1.0 (out) to 1.0 (in).";
        info->addResponse<Object<MySpeedDto>>(Status::CODE_200, "application/json");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to retrieve pump speed");
        info->addResponse<String>(Status::CODE_504, "application/json", "Request timed out");
    }
    ADD_CORS(getCuvettePumpSpeed)
    ENDPOINT("GET", "/control/cuvette_pump/speed", getCuvettePumpSpeed);

    /**
     * @brief Sets flowrate of the cuvette pump
     */
    ENDPOINT_INFO(setCuvettePumpFlowrate) {
    info->summary = "Set cuvette pump flowrate";
    info->description = "Sets the flowrate of the cuvette pump in range -1000.0 (pumping liquid out) to 1000.0 (pumping liquid in).";
    info->addTag("Control module");
    info->addConsumes<Object<MyFlowrateDto>>("application/json");
    info->addResponse<String>(Status::CODE_200, "application/json", "Flowrate set successfully.");
    info->addResponse<String>(Status::CODE_400, "application/json", "Invalid flowrate value.");
    info->addResponse<String>(Status::CODE_500, "application/json", "Failed to set flowrate.");
    }
    ADD_CORS(setCuvettePumpFlowrate)
    ENDPOINT("POST", "/control/cuvette_pump/flowrate", setCuvettePumpFlowrate, BODY_DTO(Object<MyFlowrateDto>, body));

    /**
     * @brief Retrieves current flowrate of the cuvette pump
     */
    ENDPOINT_INFO(getCuvettePumpFlowrate) {
    info->summary = "Get cuvette pump flowrate";
    info->description = "Retrieves the current flowrate of the cuvette pump. Positive value means pumping liquid in, negative value means pumping liquid out.";
    info->addTag("Control module");
    info->addResponse<Object<MyFlowrateDto>>(Status::CODE_200, "application/json");
    info->addResponse<String>(Status::CODE_500, "application/json", "Failed to retrieve flowrate");
    info->addResponse<String>(Status::CODE_504, "application/json", "Request timed out");
    }
    ADD_CORS(getCuvettePumpFlowrate)
    ENDPOINT("GET", "/control/cuvette_pump/flowrate", getCuvettePumpFlowrate);

    /**
     * @brief Moves requested amount of liquid in or out of the cuvette
     */
    ENDPOINT_INFO(moveCuvettePump) {
    info->summary = "Move cuvette pump liquid";
    info->description = "Moves the requested amount of liquid in or out of the cuvette. Positive volume means pumping liquid in, and negative volume means pumping liquid out. Flowrate is specified in ml/min and must be positive. If set to zero, the maximum flowrate of the pump will be used.";
    info->addTag("Control module");
    info->addConsumes<Object<MyMoveDto>>("application/json");
    info->addResponse<String>(Status::CODE_200, "application/json", "Movement started successfully.");
    info->addResponse<String>(Status::CODE_400, "application/json", "Invalid volume or flowrate value.");
    info->addResponse<String>(Status::CODE_500, "application/json", "Failed to start movement.");
    }
    ADD_CORS(moveCuvettePump)
    ENDPOINT("POST", "/control/cuvette_pump/move", moveCuvettePump, BODY_DTO(Object<MyMoveDto>, body));

    /**
     * @brief Primes the cuvette pump.
     */
    ENDPOINT_INFO(primeCuvettePump) {
        info->summary = "Prime cuvette pump";
        info->description = "Primes the cuvette pump by pumping liquid into the cuvette. This is used to fill the cuvette with liquid and remove air from the system.";
        info->addTag("Control module");
        info->addResponse<String>(Status::CODE_200, "application/json", "Cuvette pump priming was started.");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to start cuvette pump priming.");
    }
    ADD_CORS(primeCuvettePump)
    ENDPOINT("POST", "/control/cuvette_pump/prime", primeCuvettePump);

    /**
     * @brief Purges the cuvette pump.
     */
    ENDPOINT_INFO(purgeCuvettePump) {
        info->summary = "Purge cuvette pump";
        info->description = "Purges the cuvette pump by pumping liquid out of the cuvette. This is used to remove liquid from the cuvette and clean the system.";
        info->addTag("Control module");
        info->addResponse<String>(Status::CODE_200, "application/json", "Cuvette pump purging was started.");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to start cuvette pump purging.");
    }
    ADD_CORS(purgeCuvettePump)
    ENDPOINT("POST", "/control/cuvette_pump/purge", purgeCuvettePump);

    /**
     * @brief Stops the cuvette pump.
     */
    ENDPOINT_INFO(stopCuvettePump) {
        info->summary = "Stop cuvette pump";
        info->description = "Stops the cuvette pump immediately and disables any planned movements.";
        info->addTag("Control module");
        info->addResponse<String>(Status::CODE_200, "application/json", "Cuvette pump was stopped.");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to stop cuvette pump.");
    }
    ADD_CORS(stopCuvettePump)
    ENDPOINT("POST", "/control/cuvette_pump/stop", stopCuvettePump);

    /**
     * @brief Sets the speed of the aerator.
     */
    ENDPOINT_INFO(setAeratorSpeed) {
        info->summary = "Set aerator speed";
        info->description = "Sets the speed of the aerator in range 0.0 to 1.0.";
        info->addTag("Control module");
        info->addConsumes<Object<MySpeedDto>>("application/json");
        info->addResponse<String>(Status::CODE_200, "application/json", "Speed set successfully.");
        info->addResponse<String>(Status::CODE_400, "application/json", "Invalid speed value.");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to set aerator speed.");
    }
    ADD_CORS(setAeratorSpeed)
    ENDPOINT("POST", "/control/aerator/speed", setAeratorSpeed, BODY_DTO(Object<MySpeedDto>, body));

    /**
     * @brief Retrieves the current speed of the aerator.
     */
    ENDPOINT_INFO(getAeratorSpeed) {
        info->summary = "Get aerator speed";
        info->addTag("Control module");
        info->description = "Retrieves current speed of the aerator. Range is 0.0 to 1.0.";
        info->addResponse<Object<MySpeedDto>>(Status::CODE_200, "application/json");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to retrieve aerator speed");
        info->addResponse<String>(Status::CODE_504, "application/json", "Request timed out");
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
        info->addConsumes<Object<MyFlowrateDto>>("application/json");
        info->addResponse<String>(Status::CODE_200, "application/json", "Flowrate set successfully.");
        info->addResponse<String>(Status::CODE_400, "application/json", "Invalid flowrate value.");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to set aerator flowrate.");
    }
    ADD_CORS(setAeratorFlowrate)
    ENDPOINT("POST", "/control/aerator/flowrate", setAeratorFlowrate, BODY_DTO(Object<MyFlowrateDto>, body));

    /**
     * @brief Retrieves the current flowrate of the aerator.
     */
    ENDPOINT_INFO(getAeratorFlowrate) {
        info->summary = "Get aerator flowrate";
        info->addTag("Control module");
        info->description = "Retrieves current flowrate of the aerator. Range is 0.0 to 5000.0 ml/min.";
        info->addResponse<Object<MyFlowrateDto>>(Status::CODE_200, "application/json");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to retrieve aerator flowrate");
        info->addResponse<String>(Status::CODE_504, "application/json", "Request timed out");
    }
    ADD_CORS(getAeratorFlowrate)
    ENDPOINT("GET", "/control/aerator/flowrate", getAeratorFlowrate);

    /**
     * @brief Moves requested amount of air into the bottle using the aerator
     */
    ENDPOINT_INFO(moveAerator) {
        info->summary = "Move aerator air";
        info->description = "Moves the requested amount of air into the bottle. Volume is in ml. Flowrate is in ml/min, if set to zero, the maximal flowrate of the pump will be used.";
        info->addTag("Control module");
        info->addConsumes<Object<MyMoveDto>>("application/json");
        info->addResponse<String>(Status::CODE_200, "application/json", "Movement started successfully.");
        info->addResponse<String>(Status::CODE_400, "application/json", "Invalid volume or flowrate value.");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to start movement.");
    }
    ADD_CORS(moveAerator)
    ENDPOINT("POST", "/control/aerator/move", moveAerator, BODY_DTO(Object<MyMoveDto>, body));

    /**
     * @brief Stops the aerator and disables any planned movements.
     */
    ENDPOINT_INFO(stopAerator) {
        info->summary = "Stop aerator";
        info->description = "Stops the aerator immediately and disables any planned movements.";
        info->addTag("Control module");
        info->addResponse<String>(Status::CODE_200, "application/json", "Aerator was stopped.");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to stop aerator.");
    }
    ADD_CORS(stopAerator)
    ENDPOINT("POST", "/control/aerator/stop", stopAerator);

    /**
     * @brief Sets the speed of the mixer.
     */
    ENDPOINT_INFO(setMixerSpeed) {
        info->summary = "Set mixer speed";
        info->description = "Sets the speed of the mixer in the range [0.0, 1.0].";
        info->addTag("Control module");
        info->addConsumes<Object<MySpeedDto>>("application/json");
        info->addResponse<String>(Status::CODE_200, "application/json", "Speed set successfully.");
        info->addResponse<String>(Status::CODE_400, "application/json", "Invalid speed value.");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to set mixer speed.");
    }
    ADD_CORS(setMixerSpeed)
    ENDPOINT("POST", "/control/mixer/speed", setMixerSpeed, BODY_DTO(Object<MySpeedDto>, body));

    /**
     * @brief Retrieves the current speed of the mixer.
     */
    ENDPOINT_INFO(getMixerSpeed) {
        info->summary = "Get mixer speed";
        info->description = "Retrieves the current speed of the mixer in the range [0.0, 1.0].";
        info->addTag("Control module");
        info->addResponse<Object<MySpeedDto>>(Status::CODE_200, "application/json");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to retrieve mixer speed");
        info->addResponse<String>(Status::CODE_504, "application/json", "Request timed out");
    }
    ADD_CORS(getMixerSpeed)
    ENDPOINT("GET", "/control/mixer/speed", getMixerSpeed);

    /**
     * @brief Sets the target RPM of the mixer.
     */
    ENDPOINT_INFO(setMixerRpm) {
        info->summary = "Set mixer RPM";
        info->description = "Sets the target RPM of the mixer. Range: 0 to 10000.";
        info->addTag("Control module");
        info->addConsumes<Object<MyRpmDto>>("application/json");
        info->addResponse<String>(Status::CODE_200, "application/json", "Target RPM set successfully.");
        info->addResponse<String>(Status::CODE_400, "application/json", "Invalid RPM value.");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to set mixer RPM.");
    }
    ADD_CORS(setMixerRpm)
    ENDPOINT("POST", "/control/mixer/rpm", setMixerRpm, BODY_DTO(Object<MyRpmDto>, body));

    /**
     * @brief Retrieves the current RPM of the mixer.
     */
    ENDPOINT_INFO(getMixerRpm) {
        info->summary = "Get mixer RPM";
        info->description = "Retrieves the current RPM of the mixer. Range: 0 to 10000.";
        info->addTag("Control module");
        info->addResponse<Object<MyRpmDto>>(Status::CODE_200, "application/json");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to retrieve mixer RPM");
        info->addResponse<String>(Status::CODE_504, "application/json", "Request timed out");
    }
    ADD_CORS(getMixerRpm)
    ENDPOINT("GET", "/control/mixer/rpm", getMixerRpm);

    /**
     * @brief Sets the mixer to stir at a specified RPM for a specified time.
     */
    ENDPOINT_INFO(stirMixer) {
        info->summary = "Stir mixer";
        info->description = "Sets the mixer to stir at a specified RPM for a specified time. RPM is in RPM, time is in seconds.";
        info->addTag("Control module");
        info->addConsumes<Object<MyStirDto>>("application/json");
        info->addResponse<String>(Status::CODE_200, "application/json", "Stirring started successfully.");
        info->addResponse<String>(Status::CODE_400, "application/json", "Invalid RPM or time value.");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to start stirring.");
    }
    ADD_CORS(stirMixer)
    ENDPOINT("POST", "/control/mixer/stir", stirMixer, BODY_DTO(Object<MyStirDto>, body));

    /**
     * @brief Stops the mixer immediately.
     */
    ENDPOINT_INFO(stopMixer) {
        info->summary = "Stop mixer";
        info->description = "Stops the mixer immediately and disables any planned movements.";
        info->addTag("Control module");
        info->addResponse<String>(Status::CODE_200, "application/json", "Mixer was stopped.");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to stop the mixer.");
    }
    ADD_CORS(stopMixer)
    ENDPOINT("POST", "/control/mixer/stop", stopMixer);

    /**
     * @brief Retrieves the temperature of the bottle.
     */
    ENDPOINT_INFO(getBottleTemperature) {
        info->summary = "Retrieves temperature of the bottle";
        info->description = "Retrieves temperature of the bottle in °C.";
        info->addTag("Sensor module");
        info->addResponse<Object<MyTempDto>>(Status::CODE_200, "application/json");
        info->addResponse<String>(Status::CODE_404, "application/json", "Bottle temperature not available");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to retrieve temperature");
        info->addResponse<String>(Status::CODE_504, "application/json", "Request timed out");
    }
    ADD_CORS(getBottleTemperature)
    ENDPOINT("GET", "/sensor/bottle/temperature", getBottleTemperature);

    /**
     * @brief Retrieves the measured temperature from the top sensor of the bottle.
     */
    ENDPOINT_INFO(getTopMeasuredTemperature) {
        info->summary = "Retrieves measured temperature from top sensor";
        info->description = "Retrieves measured temperature of the top of the bottle in °C.";
        info->addTag("Sensor module");
        info->addResponse<Object<MyTempDto>>(Status::CODE_200, "application/json");
        info->addResponse<String>(Status::CODE_404, "application/json", "Top temperature not available");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to retrieve temperature");
        info->addResponse<String>(Status::CODE_504, "application/json", "Request timed out");
    }
    ADD_CORS(getTopMeasuredTemperature)
    ENDPOINT("GET", "/sensor/bottle/top/measured_temperature", getTopMeasuredTemperature);

    /**
     * @brief Retrieves the measured temperature from the bottom sensor of the bottle.
     */
    ENDPOINT_INFO(getBottomMeasuredTemperature) {
        info->summary = "Retrieves measured temperature from bottom sensor";
        info->description = "Retrieves measured temperature of the bottom of the bottle in °C.";
        info->addTag("Sensor module");
        info->addResponse<Object<MyTempDto>>(Status::CODE_200, "application/json");
        info->addResponse<String>(Status::CODE_404, "application/json", "Bottom temperature not available");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to retrieve temperature");
        info->addResponse<String>(Status::CODE_504, "application/json", "Request timed out");
    }
    ADD_CORS(getBottomMeasuredTemperature)
    ENDPOINT("GET", "/sensor/bottle/bottom/measured_temperature", getBottomMeasuredTemperature);

    /**
     * @brief Retrieves the temperature of the top sensor case of the bottle.
     */
    ENDPOINT_INFO(getTopSensorTemperature) {
        info->summary = "Retrieves temperature of the top sensor case";
        info->description = "Retrieves temperature of the sensor on top of the bottle in °C.";
        info->addTag("Sensor module");
        info->addResponse<Object<MyTempDto>>(Status::CODE_200, "application/json");
        info->addResponse<String>(Status::CODE_404, "application/json", "Top sensor temperature not available");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to retrieve temperature");
        info->addResponse<String>(Status::CODE_504, "application/json", "Request timed out");
    }
    ADD_CORS(getTopSensorTemperature)
    ENDPOINT("GET", "/sensor/bottle/top/sensor_temperature", getTopSensorTemperature);

    /**
     * @brief Retrieves the temperature of the bottom sensor case of the bottle.
     */
    ENDPOINT_INFO(getBottomSensorTemperature) {
        info->summary = "Retrieves temperature of the bottom sensor case";
        info->description = "Retrieves temperature of the sensor on bottom of the bottle in °C.";
        info->addTag("Sensor module");
        info->addResponse<Object<MyTempDto>>(Status::CODE_200, "application/json");
        info->addResponse<String>(Status::CODE_404, "application/json", "Bottom sensor temperature not available");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to retrieve temperature");
        info->addResponse<String>(Status::CODE_504, "application/json", "Request timed out");
    }
    ADD_CORS(getBottomSensorTemperature)
    ENDPOINT("GET", "/sensor/bottle/bottom/sensor_temperature", getBottomSensorTemperature);





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
// Scheduler
// ==========================================   

    /**
    * @brief Uploads and checks the syntax of a script to be run by the scheduler.
    */
    ENDPOINT_INFO(uploadScript) {
        info->summary = "Upload script to scheduler";
        info->addTag("Scheduler");
        info->description = "Upload script to scheduler. It will just upload and check syntax. It will not run the script.";
        info->addConsumes<Object<MyScriptDto>>("application/json");
        info->addResponse<String>(Status::CODE_200, "application/json", "Successfully restarted module");
        info->addResponse<String>(Status::CODE_404, "application/json", "Module not found");
    }
    ADD_CORS(uploadScript)
    ENDPOINT("PUT", "/scheduler/script", uploadScript, BODY_DTO(Object<MyScriptDto>, body));

    /**
     * @brief Retrieves the currently uploaded script.
     */
    ENDPOINT_INFO(getScript) {
        info->summary = "Get uploaded script";
        info->addTag("Scheduler");
        info->description = "Retrieves the currently uploaded script.";
        info->addResponse<Object<MyScriptDto>>(Status::CODE_200, "application/json");
        info->addResponse<String>(Status::CODE_404, "application/json", "Script not found");
    }
    ADD_CORS(getScript)
    ENDPOINT("GET", "/scheduler/script", getScript);

    /**
     * @brief Starts the scheduler.
     */
    ENDPOINT_INFO(startScheduler) {
        info->summary = "Start scheduler";
        info->addTag("Scheduler");
        info->description = "Starts the scheduler. The scheduler will run the uploaded script.";
        info->addResponse<Object<MyScriptProcessIdDto>>(Status::CODE_200, "application/json");
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
        info->addResponse<Object<MyScriptRuntimeInfoDto>>(Status::CODE_200, "application/json");
        info->addResponse<String>(Status::CODE_500, "application/json", "Failed to retrieve scheduler info");
    }
    ADD_CORS(getSchedulerInfo)
    ENDPOINT("GET", "/scheduler/runtime", getSchedulerInfo);

    

private:
    
    std::shared_ptr<ICommonModule> getModule(const oatpp::Enum<dto::ModuleEnum>::AsString& module);
    int getChannel(const dto::ChannelEnum& channel);

    std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> process(
        std::string name,
        std::function<std::shared_ptr<oatpp::web::protocol::http::outgoing::Response>()> body);

    std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> processBool(
        std::string name,
        std::function<bool()> body
    );

private:
    std::shared_ptr<ISystemModule> systemModule;
    std::shared_ptr<Scheduler> scheduler_;
};

#include OATPP_CODEGEN_END(ApiController)
