#include "SMBR/APITester.hpp"
#include "SMBR/Log.hpp"

#include "APIClientImpl.hpp"
#include <fstream>

#include <Poco/Path.h>



APITester::APITester(APIClientParams params)
    : impl(std::make_shared<APIClientImpl>(params))
{
    LTRACE("Test") << "APITester created with " << params.host << ":" << params.port << LE;
}

APITester::~APITester() {}

static void checkResponse(int code, std::string description, const std::string & message) {
    throw std::runtime_error("Error: " + std::to_string(code) + " " + description + " - " + message);
}
static void checkResponse(std::shared_ptr<oatpp::web::protocol::http::incoming::Response> response){
    if (response->getStatusCode() != 200) {
        checkResponse(response->getStatusCode(), response->getStatusDescription(), response->readBodyToString()->c_str());
    }
    
}

void APITester::capture(){

    oatpp::Object<FluorometerOjipCaptureRequestDto> body = FluorometerOjipCaptureRequestDto::createShared();

    LDEBUG("Test") << "Capture" << LE;
    auto response = impl->client()->capture(
        "x1", //gain
        "Logarithmic", //timing
        "1000", //length_ms
        "1000", //sample_count
        body
    );
    checkResponse(response);
}

bool APITester::captureDone(){
    LDEBUG("Test") << "Is Capture done?" << LE;
    auto response = impl->client()->isCaptureComplete();
    checkResponse(response);
    return true;
}

void APITester::retrieveLast(){
    LDEBUG("Test") << "Retrieve last" << LE;
    auto response = impl->client()->retrieveLast();
    checkResponse(response);
    auto captureDto = response->readBodyToDto<oatpp::Object<FluorometerOjipCaptureRequestDto>>(impl->objectMapper());
    LDEBUG("Test") << "Capture retrieved"  << LE;
}


