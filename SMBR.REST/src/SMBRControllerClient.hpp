#pragma once

#include "oatpp/web/client/ApiClient.hpp"
#include "oatpp/macro/codegen.hpp"

#include "dto/MyScriptDto.hpp"
#include "dto/MyScriptRuntimeInfoDto.hpp"
#include "dto/MyScriptProcessIdDto.hpp"

/* Begin Api Client code generation */
#include OATPP_CODEGEN_BEGIN(ApiClient)


class SMBRControllerClient : public oatpp::web::client::ApiClient {

  API_CLIENT_INIT(SMBRControllerClient)

  API_CALL("PUT", "/scheduler/script", uploadScript, BODY_DTO(Object<MyScriptDto>, body))
  API_CALL("GET", "/scheduler/script", getScript)
  API_CALL("POST", "/scheduler/start", startScheduler)
  API_CALL("POST", "/scheduler/stop", stopScheduler)
  API_CALL("GET", "/scheduler/runtime", getSchedulerInfo)
  
};

/* End Api Client code generation */
#include OATPP_CODEGEN_END(ApiClient)

