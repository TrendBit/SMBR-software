#pragma once

#include "BaseController.hpp"

#include "dto/FlowrateDto.hpp"
#include "dto/MessageDto.hpp"
#include "dto/MoveDto.hpp"
#include "dto/PumpCountDto.hpp"
#include "dto/PumpInfoDto.hpp"
#include "dto/SpeedDto.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

#undef OATPP_MACRO_API_CONTROLLER_ADD_CORS_BODY_DEFAULT_METHODS
#define OATPP_MACRO_API_CONTROLLER_ADD_CORS_BODY_DEFAULT_METHODS "GET, POST, OPTIONS, PUT, PATCH, DELETE"

/**
 * @class PumpsController
 * @brief Endpoints of the pumps module.
 */
class PumpsController : public SMBRControllerBase {
public:
    PumpsController(const std::shared_ptr<oatpp::web::mime::ContentMappers>& apiContentMappers,
                    std::shared_ptr<ISystemModule> systemModule);

    /**
     * @brief Get information about how many pumps are available in a specific instance of module.
     */
    ENDPOINT_INFO(getPumpCount) {
        info->summary = "Get information about how many pumps are available in a specific instance of module";
        info->description = 
            "Retrieve information about how many pumps are available in a specific instance of module.\n";
        info->addTag("Pumps module");
        
        auto example = PumpCountDto::createShared();
        example->pump_count = 4;
        
        info->addResponse<Object<PumpCountDto>>(Status::CODE_200, "application/json", "Successfully retrieved pump count")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Pump module not available")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Pump module not available"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve pump count")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve pump count"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getPumpCount)
    ENDPOINT("GET", "/pumps/{instance_index}/pump_count", getPumpCount, PATH(UInt8, instance_index));

    /**
     * @brief Get information about a specific pump.
     */
    ENDPOINT_INFO(getPumpInfo) {
        info->summary = "Get information about a specific pump";
        info->description = 
            "Retrieve information about a specific pump by its index.\n";
        info->addTag("Pumps module");
        
        auto example = PumpInfoDto::createShared();
        example->max_flowrate = 200.0f;
        example->min_flowrate = 10.0f;
        
        info->addResponse<Object<PumpInfoDto>>(Status::CODE_200, "application/json", "Successfully retrieved pump information")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Invalid pump index")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Invalid pump index"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Pump module not available")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Pump module not available"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve pump info")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve pump info"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getPumpInfo)
    ENDPOINT("GET", "/pumps/{instance_index}/info/{pump_index}", getPumpInfo, PATH(UInt8, instance_index), PATH(UInt8, pump_index));

    /**
     * @brief Get current speed of a specific pump.
     */
    ENDPOINT_INFO(getPumpSpeed) {
        info->summary = "Retrieves current speed of the given pump";
        info->description = 
            "Retrieves current speed of the given pump. Range -1.0 (pumping liquid out) to 1.0 (pumping liquid in).\n";
        info->addTag("Pumps module");
        
        auto example = SpeedDto::createShared();
        example->speed = -0.33f;
        
        info->addResponse<Object<SpeedDto>>(Status::CODE_200, "application/json","Successfully retrieved pump speed")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Invalid pump index")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Invalid pump index"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Pump module not available")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Pump module not available"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve pump speed")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve pump speed"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getPumpSpeed)
    ENDPOINT("GET", "/pumps/{instance_index}/speed/{pump_index}", getPumpSpeed, PATH(UInt8, instance_index), PATH(UInt8, pump_index));

    /**
     * @brief Sets speed of a specific pump.
     */
    ENDPOINT_INFO(setPumpSpeed) {
        info->summary = "Sets speed of the pump";
        info->description = 
            "Sets speed of the pump in range -1.0 (pumping liquid out) to 1.0 (pumping liquid in).\n";
        info->addTag("Pumps module");
        
        auto example = SpeedDto::createShared();
        example->speed = -0.33f;
        
        info->addConsumes<Object<SpeedDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Successfully set speed of pump")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Successfully set speed of pump"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Invalid speed or pump index value")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Invalid speed or pump index value"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Pump module not available")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Pump module not available"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to set pump speed")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to set pump speed"}}));
    }
    ADD_CORS(setPumpSpeed)
    ENDPOINT("POST", "/pumps/{instance_index}/speed/{pump_index}", setPumpSpeed, PATH(UInt8, instance_index), PATH(UInt8, pump_index), BODY_DTO(Object<SpeedDto>, body));

    /**
     * @brief Get current flow rate of a specific pump.
     */
    ENDPOINT_INFO(getPumpFlowrate) {
        info->summary = "Retrieves current flow rate of the given pump";
        info->description = 
            "Retrieves current flow rate of the given pump.\n";
        info->addTag("Pumps module");
        
        auto example = FlowrateDto::createShared();
        example->flowrate = 100.0f;
        
        info->addResponse<Object<FlowrateDto>>(Status::CODE_200, "application/json", "Successfully retrieved pump flow rate")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Invalid pump index")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Invalid pump index"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Pump module not available")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Pump module not available"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to retrieve pump flow rate")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to retrieve pump flow rate"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_504, "application/json", "Request timed out")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Request timed out"}}));
    }
    ADD_CORS(getPumpFlowrate)
    ENDPOINT("GET", "/pumps/{instance_index}/flowrate/{pump_index}", getPumpFlowrate, PATH(UInt8, instance_index), PATH(UInt8, pump_index));

    /**
     * @brief Sets flow rate of a specific pump.
     */
    ENDPOINT_INFO(setPumpFlowrate) {
        info->summary = "Sets flow rate of the selected pump";
        info->description = 
            "Sets flow rate of the selected pump in ml/min.\n"
            "Maximal flowrate is internally limited by used pump. Normally in range 20-200 ml/min.\n"
            "Maximal available flowrate can be determined from info endpoint.\n"
            "Positive value means pumping liquid in, negative value means pumping liquid out of the system.\n"
            "When this endpoint is used pump will run until interrupted with another request.\n"
            "Valid flowrate range is -1000.0 to 1000.0 ml/min, but it can be limited by pump capabilities.";
        info->addTag("Pumps module");
        
        auto example = FlowrateDto::createShared();
        example->flowrate = 10.0f;
        
        info->addConsumes<Object<FlowrateDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Successfully set flow rate of selected pump")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Successfully set flow rate of selected pump"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Invalid flow rate or pump index value")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Invalid flow rate or pump index value"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Pump module not available")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Pump module not available"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to set pump flow rate")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to set pump flow rate"}}));
    }
    ADD_CORS(setPumpFlowrate)
    ENDPOINT("POST", "/pumps/{instance_index}/flowrate/{pump_index}", setPumpFlowrate, PATH(UInt8, instance_index), PATH(UInt8, pump_index), BODY_DTO(Object<FlowrateDto>, body));

    /**
     * @brief Calibrates the given pump.
     */
    ENDPOINT_INFO(calibratePump) {
        info->summary = "Calibrates the given pump";
        info->description =
            "Sends calibrated value of flowrate per minute (ml/min) to pump in order to calibrate move and flowrate commands.\n"
            "Calibration should be used if it is necessary to achieve a dosing accuracy greater than 5 % or if the liquid has a non-standard viscosity.\n"
            "Value of calibration can be checked by using GET request to info endpoint of pump.\n"
            "See cuvette pump calibration endpoint for more details.\n"
            "Valid flowrate range is 0.0 to 1000.0 ml/min.";
        info->addTag("Pumps module");

        auto example = FlowrateDto::createShared();
        example->flowrate = 30.0f;

        info->addConsumes<Object<FlowrateDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Successfully wrote pump max flowrate calibration")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Successfully wrote pump max flowrate calibration"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Invalid flowrate or pump index value")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Invalid flowrate or pump index value"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Pump module not available")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Pump module not available"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to write pump max flowrate calibration")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to write pump max flowrate calibration"}}));
    }
    ADD_CORS(calibratePump)
    ENDPOINT("POST", "/pumps/{instance_index}/calibration/{pump_index}", calibratePump, PATH(UInt8, instance_index), PATH(UInt8, pump_index), BODY_DTO(Object<FlowrateDto>, body));

    /**
     * @brief Moves requested amount of liquid by specified pump.
     */
    ENDPOINT_INFO(movePump) {
        info->summary = "Moves requested amount of liquid by specified pump";
        info->description = 
            "Moves requested amount of liquid by specified pump in ml.\n"
            "Positive value means pumping liquid in, negative value means pumping liquid out of the system.\n"
            "When this endpoint is used pump will run until requested amount of liquid is moved.\n"
            "Pump can be stopped because is done by stop endpoint.\n"
            "Valid volume range is -1000.0 to 1000.0 ml. Valid flowrate range is 0.0 to 1000.0 ml/min.";
        info->addTag("Pumps module");
        
        auto example = MoveDto::createShared();
        example->volume = 10.0f;
        example->flowrate = 2.0f;
        
        info->addConsumes<Object<MoveDto>>("application/json")
            .addExample("application/json", example);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Successfully started moving pump")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Successfully started moving pump"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Invalid volume or flowrate or pump index value")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Invalid volume or flowrate or pump index value"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Pump module not available")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Pump module not available"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to move pump")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to move pump"}}));
    }
    ADD_CORS(movePump)
    ENDPOINT("POST", "/pumps/{instance_index}/move/{pump_index}", movePump, PATH(UInt8, instance_index), PATH(UInt8, pump_index), BODY_DTO(Object<MoveDto>, body));

    /**
     * @brief Stops the given pump.
     */
    ENDPOINT_INFO(stopPump) {
        info->summary = "Stops the given pump";
        info->description = 
            "Stops the given pump.\n";
        info->addTag("Pumps module");
        
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Successfully stopped pump")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Successfully stopped pump"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Invalid pump index")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Invalid pump index"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Pump module not available")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Pump module not available"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to stop pump")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to stop pump"}}));
    }
    ADD_CORS(stopPump)
    ENDPOINT("GET", "/pumps/{instance_index}/stop/{pump_index}", stopPump, PATH(UInt8, instance_index), PATH(UInt8, pump_index));

    /**
     * @brief Sets the instance of a pump module identified by UID.
     */
    ENDPOINT_INFO(setPumpInstance) {
        info->summary = "Sets the instance of a pump module identified by UID";
        info->description = 
            "Sets the instance of the pump module with the specified UID to the target instance.\n"
            "This uses the Enumerator_set message to configure the module instance.\n";
        info->addTag("Pumps module");
        
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Successfully set pump instance")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Successfully set pump instance"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_400, "application/json", "Invalid instance index or UID")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Invalid instance index or UID"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Pump module with specified UID not found")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Pump module with specified UID not found"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_500, "application/json", "Failed to set pump instance")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Failed to set pump instance"}}));
    }
    ADD_CORS(setPumpInstance)
    ENDPOINT("POST", "/pumps/enumeration/{uid}/set-instance/{instance_index}", setPumpInstance, PATH(String, uid), PATH(UInt8, instance_index));

private:
    uint8_t pumpCountForInstance(Instance instance);
    void validatePumpIndex(Instance instance, uint8_t pump_index);

};

#include OATPP_CODEGEN_END(ApiController)
