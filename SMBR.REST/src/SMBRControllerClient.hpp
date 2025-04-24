#pragma once

#include "oatpp/web/client/ApiClient.hpp"
#include "oatpp/macro/codegen.hpp"

#include "dto/ScriptDto.hpp"
#include "dto/ScriptRuntimeInfoDto.hpp"
#include "dto/ScriptProcessIdDto.hpp"
#include "dto/CaptureEnumDto.hpp"
#include "dto/FluorometerOjipCaptureRequestDto.hpp"

/* Begin Api Client code generation */
#include OATPP_CODEGEN_BEGIN(ApiClient)


class SMBRControllerClient : public oatpp::web::client::ApiClient {

  API_CLIENT_INIT(SMBRControllerClient)

  API_CALL("PUT", "/recipes/{recipeName}", replaceRecipe, PATH(String, recipeName), BODY_DTO(Object<ScriptDto>, body))
  API_CALL("POST", "/scheduler/recipe", selectRecipe, BODY_DTO(String, recipeName))
  //API_CALL("PUT", "/scheduler/script", uploadScript, BODY_DTO(Object<ScriptDto>, body))
  API_CALL("GET", "/scheduler/recipe", getRecipe)
  API_CALL("POST", "/scheduler/start", startScheduler)
  API_CALL("POST", "/scheduler/stop", stopScheduler)
  API_CALL("GET", "/scheduler/runtime", getSchedulerInfo)

//TESTING CALLS

  API_CALL("POST", "/sensor/fluorometer/ojip/capture", capture,
        QUERY(oatpp::String, gain), 
        QUERY(oatpp::String, timing),
        QUERY(oatpp::String, length_ms),
        QUERY(oatpp::String, sample_count),
        BODY_DTO(Object<FluorometerOjipCaptureRequestDto>, body));
  API_CALL("GET", "/sensor/fluorometer/ojip/completed", isCaptureComplete);
  API_CALL("GET", "/sensor/fluorometer/ojip/read_last", retrieveLast);
};

/* End Api Client code generation */
#include OATPP_CODEGEN_END(ApiClient)

