#include "CoreController.hpp"

#include "SMBR/Exceptions.hpp"
#include "SMBR/Log.hpp"
#include "ControllerUtils.hpp"

#include <chrono>

using namespace std::chrono_literals;

CoreController::CoreController(const std::shared_ptr<oatpp::web::mime::ContentMappers>& apiContentMappers,
                               std::shared_ptr<ISystemModule> systemModule)
    : SMBRControllerBase(apiContentMappers, systemModule)
{}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> CoreController::getShortID() {

    return process(__FUNCTION__, [&](){
        auto sid = waitFor(systemModule->coreModule()->getShortID());
        auto sidResponseDto = SIDDto::createShared();
        sidResponseDto->sid = sid;
        return createDtoResponse(Status::CODE_200, sidResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> CoreController::getIpAddress() {

    return process(__FUNCTION__, [&](){
        auto ipAddress = waitFor(systemModule->coreModule()->getIpAddress());
        auto ipResponseDto = IpDto::createShared();
        ipResponseDto->ipAddress = ipAddress;
        return createDtoResponse(Status::CODE_200, ipResponseDto);
    });
}


std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> CoreController::getHostname() {

    return process(__FUNCTION__, [&](){
        auto hostname = waitFor(systemModule->coreModule()->getHostname());
        auto hostnameResponseDto = HostnameDto::createShared();
        hostnameResponseDto->hostname = hostname;
        return createDtoResponse(Status::CODE_200, hostnameResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> CoreController::getSerialNumber() {

    return process(__FUNCTION__, [&](){
        auto serial = waitFor(systemModule->coreModule()->getSerialNumber());
        auto serialResponseDto = SerialDto::createShared();
        serialResponseDto->serial = serial;
        return createDtoResponse(Status::CODE_200, serialResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> CoreController::getModel() {
    return process(__FUNCTION__, [&]() {
        auto model = waitFor(systemModule->coreModule()->getModel());
        auto modelDto = ModelDto::createShared();
        modelDto->model = model;
        return createDtoResponse(Status::CODE_200, modelDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> CoreController::getPowerSupplyType() {

    return process(__FUNCTION__, [&](){
        auto res = waitFor(systemModule->coreModule()->getPowerSupplyType());

        auto supplyTypeResponseDto = SupplyTypeDto::createShared();
        supplyTypeResponseDto->vin = res.isVIN;
        supplyTypeResponseDto->poe = res.isPoE;
        supplyTypeResponseDto->poe_hb = res.isPoE_Hb;
        return createDtoResponse(Status::CODE_200, supplyTypeResponseDto);

    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> CoreController::getVoltage5V() {

    return process(__FUNCTION__, [&](){
        auto voltage = waitFor(systemModule->coreModule()->getVoltage5V());
        auto voltageResponseDto = VoltageDto::createShared();
        voltageResponseDto->voltage = voltage;
        return createDtoResponse(Status::CODE_200, voltageResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> CoreController::getVoltageVIN() {

    return process(__FUNCTION__, [&](){
        auto voltage = waitFor(systemModule->coreModule()->getVoltageVIN());
        auto voltageResponseDto = VoltageDto::createShared();
        voltageResponseDto->voltage = voltage;
        return createDtoResponse(Status::CODE_200, voltageResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> CoreController::getPoEVoltage() {

    return process(__FUNCTION__, [&](){
        auto voltage = waitFor(systemModule->coreModule()->getVoltagePoE());
        auto voltageResponseDto = VoltageDto::createShared();
        voltageResponseDto->voltage = voltage;
        return createDtoResponse(Status::CODE_200, voltageResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> CoreController::getCurrentConsumption() {

    return process(__FUNCTION__, [&](){
        auto current = waitFor(systemModule->coreModule()->getCurrentConsumption());
        auto currentResponseDto = CurrentDto::createShared();
        currentResponseDto->current = current;
        return createDtoResponse(Status::CODE_200, currentResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> CoreController::getPowerDraw() {

    return process(__FUNCTION__, [&](){
        auto powerDraw = waitFor(systemModule->coreModule()->getPowerDraw());
        auto powerDrawResponseDto = PowerDrawDto::createShared();
        powerDrawResponseDto->power_draw = powerDraw;
        return createDtoResponse(Status::CODE_200, powerDrawResponseDto);
    });
}
