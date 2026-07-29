#include "CoreController.hpp"

#include "SMBR/Exceptions.hpp"
#include "SMBR/Log.hpp"
#include "ControllerUtils.hpp"

#include <Poco/Process.h>

#include <cctype>
#include <chrono>
#include <filesystem>
#include <fstream>

using namespace std::chrono_literals;

namespace {
    bool isValidHostname(const std::string& hostname) {
        if (hostname.empty() || hostname.size() > 8) return false;
        if (hostname.front() == '-' || hostname.back() == '-') return false;
        for (char c : hostname) {
            if (!std::isalnum(static_cast<unsigned char>(c)) && c != '-' && c != '_') return false;
        }
        return true;
    }
}

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

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> CoreController::setHostname(const oatpp::Object<HostnameRequestDto>& body) {

    return process(__FUNCTION__, [&](){
        if (!body || !body->hostname) {
            throw ArgumentException("hostname is required");
        }
        std::string hostname = body->hostname;
        if (!isValidHostname(hostname)) {
            throw ArgumentException("hostname must be 1-8 characters long and contain only letters, digits, hyphens and underscores");
        }

        std::filesystem::path hostnameFile("/data/etc/hostname");
        try {
            std::filesystem::create_directories(hostnameFile.parent_path());
        } catch (std::exception& e) {
            throw std::runtime_error("Failed to create directory " + hostnameFile.parent_path().string() + ": " + e.what());
        }

        std::ofstream file(hostnameFile, std::ios::trunc);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open " + hostnameFile.string() + " for writing");
        }
        file << hostname << "\n";
        file.close();
        if (file.fail()) {
            throw std::runtime_error("Failed to write hostname to " + hostnameFile.string());
        }

        try {
            Poco::Process::Args args{"-c", "sleep 1 && systemctl reboot"};
            Poco::Process::launch("sh", args);
        } catch (std::exception& e) {
            throw std::runtime_error("Hostname was set, but failed to trigger reboot: " + std::string(e.what()));
        }

        auto dto = MessageDto::createShared();
        dto->message = "Hostname set to '" + hostname + "'. Device is rebooting for the change to take effect.";
        return createDtoResponse(Status::CODE_200, dto);
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
