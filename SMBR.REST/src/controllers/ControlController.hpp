#pragma once

#include "BaseController.hpp"

#include "dto/AeratorInfoDto.hpp"
#include "dto/ChannelEnum.hpp"
#include "dto/CuvettePumpInfoDto.hpp"
#include "dto/FlowrateDto.hpp"
#include "dto/IntensitiesDto.hpp"
#include "dto/IntensityDto.hpp"
#include "dto/MessageDto.hpp"
#include "dto/MixerInfoDto.hpp"
#include "dto/MoveDto.hpp"
#include "dto/RpmDto.hpp"
#include "dto/SpeedDto.hpp"
#include "dto/StirDto.hpp"
#include "dto/TempDto.hpp"
#include "dto/TempNullDto.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

#undef OATPP_MACRO_API_CONTROLLER_ADD_CORS_BODY_DEFAULT_METHODS
#define OATPP_MACRO_API_CONTROLLER_ADD_CORS_BODY_DEFAULT_METHODS "GET, POST, OPTIONS, PUT, PATCH, DELETE"

/**
 * @class ControlController
 * @brief Endpoints of the control module.
 */
class ControlController : public SMBRControllerBase {
public:
    ControlController(const std::shared_ptr<oatpp::web::mime::ContentMappers>& apiContentMappers,
                      std::shared_ptr<ISystemModule> systemModule);

    /**
    * @brief Sets all channels of LED panel to given intensity.
    */
    ENDPOINT_INFO(setIntensities) {
        info->summary = "Sets all channels of LED panel to given intensity";
        info->description = 
            "Sets all channels of LED panel to respective intensities at once."
            "In API is this request split into several CAN messages settings individual channels."
            "Valid intensity range for all channels is 0.0 to 1.0.";
        info->addTag("Control module");  

        auto example = IntensitiesDto::createShared();
        auto intensityValues = std::vector<oatpp::Float32>{0.0f, 0.5f, 0.9f, 1.0f};
        example->intensity = oatpp::Vector<oatpp::Float32>::createShared();
        example->intensity->assign(intensityValues.begin(), intensityValues.end());
  
        info->addConsumes<Object<IntensitiesDto>>("application/json")
            .addExample("application/json", example);  
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Intensity set successfully")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Intensity set successfully"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Invalid request body")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Invalid request body"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to set intensity")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to set intensity"}}));
    }
    ADD_CORS(setIntensities)
    ENDPOINT("POST", "/control/led_panel/intensity", setIntensities, BODY_DTO(Object<IntensitiesDto>, body));      

    /**
    * @brief Sets the intensity and the channel of the LED lighting.
    */
    ENDPOINT_INFO(setIntensity) {
        info->summary = "Sets the selected channel of LED panel to given intensity";
        info->description = "Sets the selected channel of LED panel to given intensity. Valid intensity range is 0.0 to 1.0.";
        info->addTag("Control module");
        auto example = IntensityDto::createShared();
        example->intensity = 0.5f;
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
        info->description = "Retrieves current intensity of selected channel of LED panel.";
        info->addTag("Control module");
        
        auto example = IntensityDto::createShared();
        example->intensity = 0.5f;
        info->addResponse<Object<IntensityDto>>(Status::CODE_200, "application/json", "Successfully retrieved illumination LED channel intensity")
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
        info->summary = "Retrieves temperature of LED panel";
        info->description = "Retrieves temperature of LED panel in °C.";
        info->addTag("Control module");
        auto example = TempDto::createShared();
        example->temperature = 30.2f;
        info->addResponse<Object<TempDto>>(Status::CODE_200, "application/json", "Successfully retrieved temperature of LED panel")
            .addExample("application/json", example);
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
        info->summary = "Sets the intensity of heating or cooling";
        info->description = 
            "Sets the intensity of heating or cooling in range -1.0 (cooling) to 1.0 (heating)."
            "This value can be overwritten by regulation algorithm if temperature regulation (target temperature) is set.";
        info->addTag("Control module");
        auto example = IntensityDto::createShared();
        example->intensity = -0.33f; 
        info->addConsumes<Object<IntensityDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Successfully set intensity of heater")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Successfully set intensity of heater"}}));
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
        info->summary = "Retrieves current intensity of heating or cooling";
        info->description = 
            "Retrieves current intensity of heating or cooling. Range is -1.0 (cooling) to 1.0 (heating)."
            "Intensity can be modified by regulation algorithm if temperature regulation (target temperature) is set.";
        info->addTag("Control module"); 
        auto example = IntensityDto::createShared();
        example->intensity = -0.33f;
        info->addResponse<Object<IntensityDto>>(Status::CODE_200, "application/json", "Successfully retrieved heater intensity")
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
        info->summary = "Sets the target temperature for the heater (temperature of bottle)";
        info->description = 
            "Sets the target temperature for the heater (temperature of bottle)."
            "Temperature is in ˚C and heater will set intensity in order to reach this temperature."
            "Valid temperature range is 0.0 to 60.0 ˚C.";
        info->addTag("Control module");
        auto example = TempDto::createShared();
        example->temperature = 30.5f; 
        info->addConsumes<Object<TempDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Successfully set target temperature")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Successfully set target temperature"}}));
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
        info->summary = "Retrieves currently set target temperature for the heater (temperature of bottle)";
        info->description = "Retrieves currently set target temperature for the heater (temperature of bottle).";
        info->addTag("Control module");
        
        auto example = TempDto::createShared();
        example->temperature = 30.5f; 
        info->addResponse<Object<TempDto>>(Status::CODE_200, "application/json", "Successfully retrieved target temperature")
            .addExample("application/json", example);
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
        info->summary = "Retrieves temperature of heater plate (metal heatspreader)";
        info->description = 
            "Retrieves temperature of plate (metal heatspreader) which is controlling temperature of bottle."
            "Sensor is thermistor connected from back side of heater plate.";
        info->addTag("Control module");
        auto example = TempDto::createShared();
        example->temperature = 30.5f; 
        info->addResponse<Object<TempDto>>(Status::CODE_200, "application/json", "Successfully retrieved temperature of heater plate")
            .addExample("application/json", example);
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
        info->description = "Turn off heater by setting intensity to 0.0 and disabling temperature regulation.";
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
        info->description = 
            "Units: ml/min. Retrieves information about the cuvette pump capabilities."
            "This includes maximum and minimum flowrate of the cuvette pump at nominal conditions."
            "Cuvette pump maximal and minimal flowrates can vary based on used tubing. In those cases pump can be calibrated using calibration endpoint.";
        info->addTag("Control module");
        auto example = CuvettePumpInfoDto::createShared();
        example->max_flowrate = 200.0f;
        example->min_flowrate = 10.0f;
        info->addResponse<Object<CuvettePumpInfoDto>>(Status::CODE_200, "application/json", "Successfully retrieved cuvette pump information")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve cuvette pump information")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve cuvette pump information"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getCuvettePumpInfo)
    ENDPOINT("GET", "/control/cuvette_pump/info", getCuvettePumpInfo);



    /**
     * @brief Sets the speed of the cuvette pump.
     */
    ENDPOINT_INFO(setCuvettePumpSpeed) {
        info->summary = "Sets speed of the cuvette pump";
        info->description = "Sets speed of the cuvette pump in range -1.0 (pumping liquid out) to 1.0 (pumping liquid in).";
        info->addTag("Control module");
        auto example = SpeedDto::createShared();
        example->speed = -0.33f;
        info->addConsumes<Object<SpeedDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Successfully set speed of cuvette pump")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Successfully set speed of cuvette pump"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Invalid speed value")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Invalid speed value"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to set speed of cuvette pump")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to set speed of cuvette pump"}}));
    }
    ADD_CORS(setCuvettePumpSpeed)
    ENDPOINT("POST", "/control/cuvette_pump/speed", setCuvettePumpSpeed, BODY_DTO(Object<SpeedDto>, body));

    /**
     * @brief Retrieves the current speed of the cuvette pump.
     */
    ENDPOINT_INFO(getCuvettePumpSpeed) {
        info->summary = "Retrieves current speed of the cuvette pump";
        info->description = "Retrieves current speed of the cuvette pump. Range .0 (pumping liquid out) to 1.0 (pumping liquid in).";
        info->addTag("Control module");
        auto example = SpeedDto::createShared();
        example->speed = -0.33f;
        info->addResponse<Object<SpeedDto>>(Status::CODE_200, "application/json", "Successfully retrieved cuvette pump speed")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve cuvette pump speed")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve cuvette pump speed"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getCuvettePumpSpeed)
    ENDPOINT("GET", "/control/cuvette_pump/speed", getCuvettePumpSpeed);

    /**
     * @brief Sets flowrate of the cuvette pump
     */
    ENDPOINT_INFO(setCuvettePumpFlowrate) {
        info->summary = "Sets flowrate of the cuvette pump";
        info->description = 
            "Sets flowrate of the cuvette pump in ml/min. Maximal flowrate is internally limited by used pump. Normaly in range 20-100 ml/min."
            "Current limit of pump can be obtained from info endpoint. Positive value means pumping liquid in, negative value means pumping liquid out of the cuvette."
            "When this endpoint is used pump will run until interrupted with another request."
            "Pump does not have feedback loop, so flowrate is not guaranteed but extrapolated from measured data."
            "Valid flowrate range is -1000.0 to 1000.0 ml/min.";
        info->addTag("Control module");
        auto example = FlowrateDto::createShared();
        example->flowrate = 10.0f; 
        info->addConsumes<Object<FlowrateDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Successfully set flowrate of cuvette pump")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Successfully set flowrate of cuvette pump"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Invalid flowrate value")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Invalid flowrate value"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to set flowrate of cuvette pump")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to set flowrate of cuvette pump"}}));
    }
    ADD_CORS(setCuvettePumpFlowrate)
    ENDPOINT("POST", "/control/cuvette_pump/flowrate", setCuvettePumpFlowrate, BODY_DTO(Object<FlowrateDto>, body));

    /**
     * @brief Retrieves current flowrate of the cuvette pump
     */
    ENDPOINT_INFO(getCuvettePumpFlowrate) {
        info->summary = "Retrieves current flowrate of the cuvette pump";
        info->description = 
            "Retrieves current flowrate of the cuvette pump. Positive value means pumping liquid in, negative value means pumping liquid out of the cuvette."
            "This can be used anytime to check if pump is still running and in which direction." 
            "Pump does not have feedback loop, so flowrate is not guaranteed but extrapolated from measured data.";
        info->addTag("Control module");
        auto example = FlowrateDto::createShared();
        example->flowrate = 10.0f;
        info->addResponse<Object<FlowrateDto>>(Status::CODE_200, "application/json", "Successfully retrieved cuvette pump flowrate")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve cuvette pump flowrate")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve cuvette pump flowrate"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getCuvettePumpFlowrate)
    ENDPOINT("GET", "/control/cuvette_pump/flowrate", getCuvettePumpFlowrate);

    /**
     * @brief Moves requested amount of liquid in or out of the cuvette
     */
    ENDPOINT_INFO(moveCuvettePump) {
        info->summary = "Moves requested amount of liquid in or out of the cuvette";
        info->description = 
            "Moves requested amount of liquid in or out of the cuvette."
            "Amount of liquid is in ml, positive value means pumping liquid in, negative value means pumping liquid out of the cuvette."
            "Flowrate can be specified in ml/min and must be positive, if set to zero then maximal flowrate of pump will be used."
            "Flowrate is limited by current pump capabilities. This can be checked using the info endpoint."
            "Status of movement can be checked by using GET request to flowrate or speed endpoints."
            "Valid volume range is 0.0 to 1000.0 ml. Valid flowrate range is -1000.0 to 1000.0 ml/min.";
        info->addTag("Control module");
    
        auto example = MoveDto::createShared();
        example->volume = 10.0f; 
        example->flowrate = 2.0f; 
        info->addConsumes<Object<MoveDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Successfully started moving liquid with cuvette pump")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Successfully started moving liquid with cuvette pump"}}));
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
        info->summary = "Stops the cuvette pump";
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
            "- Send measured volume/flowrate to the module. Unit is ml/min.\n\n"
            "Common value is around 10 to 100 ml/min, default is 30 ml/min."
            "Valid flowrate range is 0.0 to 1000.0 ml/min.";
        info->addTag("Control module");
        auto example = FlowrateDto::createShared();
        example->flowrate = 30.0;
        info->addConsumes<Object<FlowrateDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Successfully wrote cuvette pump max flowrate calibration")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Successfully wrote cuvette pump max flowrate calibration"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Invalid flowrate value")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Invalid flowrate value"}}));
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
        info->description = 
            "Units: ml/min. Retrieves information about the aerator capabilities."
            "This includes maximum and minimum flowrate of the aerator at nominal conditions."
            "Aerator maximal and minimal flowrates can be different based on used tubing and their lengths."
            "This endpoints reflect changes made by calibration endpoint.";
        info->addTag("Control module");
        auto example = AeratorInfoDto::createShared();
        example->max_flowrate = 2500.0f;
        example->min_flowrate = 10.0f;
    
        info->addResponse<Object<AeratorInfoDto>>(Status::CODE_200, "application/json", "Successfully retrieved aerator information")
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
        info->summary = "Sets speed of the aerator (air pump)";
        info->description = "Sets speed of the aerator in range 0.0 to 1.0. Aerator can only pump air into the bottle, so negative values are not allowed.";
        info->addTag("Control module");
        auto example = SpeedDto::createShared();
        example->speed = 0.5f;
        info->addConsumes<Object<SpeedDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Successfully set speed of aerator")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Successfully set speed of aerator"}}));
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
        info->summary = "Retrieves current speed of the aerator (air pump)";
        info->description = "Retrieves current speed of the aerator. Range 0.0 to 1.0.";
        info->addTag("Control module");
        auto example = SpeedDto::createShared();
        example->speed = 0.5f;
        info->addResponse<Object<SpeedDto>>(Status::CODE_200, "application/json", "Successfully retrieved aerator speed")
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
        info->summary = "Sets flowrate of the aerator (air pump)";
        info->description = 
            "Sets flowrate of the aerator in ml/min. Normaly in range 100-2500 ml/min."
            "Flowrate can be only set in supported range (obtained from info endpoint)."
            "Setting flowrate outside of this will result in warning and value will be clamped into supported range."
            "Lower then minimal supported minimal flowrate can be achieved by setting low speed."
            "Flowrate is internally limited by used pump."
            "Positive value means pumping air into the bottle. Pumping out is not possible with aerator"
            "When this endpoint is used, pump will run until interrupted with another request."
            "Aerator does not have feedback loop. Flowrate is not guaranteed but extrapolated from measured data."
            "Due to this, real flowrate can be different when using longer tubes."
            "In those cases aerator can be calibrated using calibration endpoint."
            "Valid flowrate range is 10.0 to 5000.0 ml/min.";
        info->addTag("Control module");
        auto example = FlowrateDto::createShared();
        example->flowrate = 100.0f;
        info->addConsumes<Object<FlowrateDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Successfully set flowrate of aerator")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Successfully set flowrate of aerator"}}));
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
        info->summary = "Retrieves current flowrate of the aerator (air pump)";
        info->description = 
            "Retrieves current flowrate of the aerator in ml/min. Positive value means pumping air into the bottle."
            "This can be used anytime to check if pump is still running."
            "Aerator does not have feedback loop. Flowrate is not guaranteed but extrapolated from measured data."
            "Due to this, real flowrate can be different when using longer tubes."
            "When flowrate/speed is out of supported range, this value could be far from reality.";
        info->addTag("Control module");
        auto example = FlowrateDto::createShared();
        example->flowrate = 100.0f;
        info->addResponse<Object<FlowrateDto>>(Status::CODE_200, "application/json", "Successfully retrieved aerator flowrate")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve aerator flowrate")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve aerator flowrate"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getAeratorFlowrate)
    ENDPOINT("GET", "/control/aerator/flowrate", getAeratorFlowrate);;

    /**
     * @brief Configure measured volume on module in order to calibrate aerator flowrate.
     */
    ENDPOINT_INFO(calibrateAerator) {
        info->summary = "Configure measured volume on module in order to calibrate aerator flowrate";
        info->description =
            "Sends calibrated value of flowrate per minute (ml/min) to aerator in order to calibrate move and flowrate commands.\n"
            "Calibration should be used if it is necessary to achieve a dosing accuracy greater than 20 %.\n"
            "Value of calibration can be checked by using GET request to info endpoint of aerator.\n"
            "Calibration send by this endpoint is stored on module and used after restart.\n"
            "Valid flowrate range is 0.0 to 1000.0 ml/min.";
        info->addTag("Control module");

        auto example = FlowrateDto::createShared();
        example->flowrate = 30.0f;

        info->addConsumes<Object<FlowrateDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Successfully wrote cuvette pump max flowrate calibration")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Successfully wrote cuvette pump max flowrate calibration"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Invalid flowrate value")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Invalid flowrate value"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to set calibration")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to set calibration"}}));
    }
    ADD_CORS(calibrateAerator)
    ENDPOINT("POST", "/control/aerator/calibration", calibrateAerator, BODY_DTO(Object<FlowrateDto>, body));

    /**
     * @brief Moves requested amount of air into the bottle using the aerator
     */
    ENDPOINT_INFO(moveAerator) {
        info->summary = "Moves requested amount of air into the bottle";
        info->description = 
            "Moves requested amount of air into the bottle. Amount of air is in ml."
            "Flowrate can be specified in ml/min and must be positive, if set to zero then maximal flowrate of pump will be used."
            "Status of movement can be checked by using GET request to flowrate or speed endpoints."
            "Valid volume range is 0.0 to 1000.0 ml. Valid flowrate range is 10.0 to 5000.0 ml/min.";
        info->addTag("Control module");
        auto example = MoveDto::createShared();
        example->volume = 100.0f;
        example->flowrate = 100.0f;
        info->addConsumes<Object<MoveDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Successfully started moving air with aerator")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Successfully started moving air with aerator"}}));
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
        info->summary = "Stops the aerator (air pump)";
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
        info->description = "Units: revolutions per minute (RPM). Retrieves information about the mixer capabilities."
            "This includes maximum and minimum reliable RPM of the mixer at normal conditions."
            "Mixer maximal and minimal speeds can be different based on used magnetic stirrer and liquid density."
            "Lower then minimal values can be set.";
        info->addTag("Control module");
        auto example = MixerInfoDto::createShared();
        example->max_rpm = 5000;
        example->min_rpm = 1000;

        info->addResponse<Object<MixerInfoDto>>(Status::CODE_200, "application/json", "Successfully retrieved mixer information")
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
        info->summary = "Sets speed of the mixer";
        info->description = "Sets speed of the mixer in range 0.0 to 1.0.";
        info->addTag("Control module");
        auto example = SpeedDto::createShared();
        example->speed = 0.5f; 
        info->addConsumes<Object<SpeedDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Successfully set speed of mixer")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Successfully set speed of mixer"}}));
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
        info->summary = "Retrieves current speed of the mixer";
        info->description = "Retrieves current speed of the mixer. Range: 0.0 to 1.0.";
        info->addTag("Control module");
        auto example = SpeedDto::createShared();
        example->speed = 0.5f; 
        info->addResponse<Object<SpeedDto>>(Status::CODE_200, "application/json", "Successfully retrieved mixer speed")
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
        info->summary = "Sets target RPM of the mixer";
        info->description = 
            "Sets target RPM of the mixer. Real minimal and maximal values are limited by used stirrer element and liquid density."
            "Mixer will try to achieve and hold this RPM but it is not guaranteed that it will be reached."
            "Maximal RPM depends on used magnetic stirrer and liquid density."
            "Speed ramping can take some time so mixer can reach maximal RPM in several seconds."
            "Valid rpm range is 0.0 to 10000.0 RPM.";
        info->addTag("Control module");
        auto example = RpmDto::createShared();
        example->rpm = 3000.0f; 
        info->addConsumes<Object<RpmDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Successfully set target RPM of mixer")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Successfully set target RPM of mixer"}}));
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
        info->summary = "Retrieves current RPM of the mixer";
        info->description = "Retrieves current RPM of the mixer.";
        info->addTag("Control module");
        auto example = RpmDto::createShared();
        example->rpm = 500.0f;
        info->addResponse<Object<RpmDto>>(Status::CODE_200, "application/json", "Successfully retrieved mixer RPM")
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
        info->summary = "Sets the mixer to stir at a specified RPM for a specified time";
        info->description = 
            "Sets the mixer to stir at a specified RPM for a specified time." 
            "RPM is the speed of the mixer and time is the duration in seconds."
            "Valid rpm range is 0.0 to 10000.0 RPM. Valid time range is 0.0 to 3600.0 seconds.";
        info->addTag("Control module");
        auto example = StirDto::createShared();
        example->rpm = 3000.0f; 
        example->time = 60.0f; 
        info->addConsumes<Object<StirDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Successfully set mixer to stir")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Successfully set mixer to stir"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Invalid RPM or time value")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Invalid RPM or time value"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to set mixer to stir")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to set mixer to stir"}}));
    }
    ADD_CORS(stirMixer)
    ENDPOINT("POST", "/control/mixer/stir", stirMixer, BODY_DTO(Object<StirDto>, body));

    /**
     * @brief Stops the mixer immediately.
     */
    ENDPOINT_INFO(stopMixer) {
        info->summary = "Stops the mixer";
        info->description = "Stops the mixer immediately and disables any planned movements.";
        info->addTag("Control module");
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Mixer was stopped")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Mixer was stopped"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to stop the mixer")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to stop the mixer"}}));
    }
    ADD_CORS(stopMixer)
    ENDPOINT("GET", "/control/mixer/stop", stopMixer);

private:
    int getChannel(const dto::ChannelEnum& channel);

};

#include OATPP_CODEGEN_END(ApiController)
