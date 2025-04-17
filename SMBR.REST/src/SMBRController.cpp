#include "SMBRController.hpp"
#include "SMBR/Exceptions.hpp"
#include <Poco/DateTimeFormatter.h>

#include "SMBR/Recipes.hpp"
#include "SMBR/Scheduler.hpp"
#include "SMBR/Log.hpp"

#include <chrono>

using namespace std::chrono_literals;

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::process(
    std::string name,
    std::function<std::shared_ptr<oatpp::web::protocol::http::outgoing::Response>()> body
){
    try {
        LDEBUG("API") << "Api " << name << " begin" << LE;
        auto b = body();
        LDEBUG("API") << "Api " << name << " end" << LE;
        return b;
    } catch (TimeoutException & e){
        auto dto = MessageDto::createShared();
        dto->message = name + " timed out: " + std::string(e.what());
        LWARNING("API") << "Api " << name << " failed with TimeoutException " << std::string(e.what()) << LE;
        return createDtoResponse(Status::CODE_504, dto);
    } catch (NotFoundException & e){
        auto dto = MessageDto::createShared();
        dto->message = name + " not found: " + std::string(e.what());
        LWARNING("API") << "Api " << name << " failed with NotFoundException " << std::string(e.what()) << LE;
        return createDtoResponse(Status::CODE_404, dto);
    } catch (std::exception & e){
        auto dto = MessageDto::createShared();
        dto->message = "Failed to retrieve " + name + ": " + std::string(e.what());
        LWARNING("API") << "Api " << name << " failed with Exception " << std::string(e.what()) << LE;
        return createDtoResponse(Status::CODE_500, dto);
    }
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::processBool(
    std::string name,
    std::function<bool()> body
){
    try {
        LDEBUG("API") << "Api " << name << " begin" << LE;
        bool success = body();
        auto dto = MessageDto::createShared();
        if (success) {
            dto->message = name + " successful";
            LDEBUG("API") << "Api " << name << " end (return value = true)" << LE;
            return createDtoResponse(Status::CODE_200, dto);
        } else {
            dto->message = name + " failed";
            LDEBUG("API") << "Api " << name << " end (return value = false)" << LE;
            return createDtoResponse(Status::CODE_500, dto);
        }
    } catch (TimeoutException & e){
        auto dto = MessageDto::createShared();
        dto->message = name + " timed out: " + std::string(e.what());
        LWARNING("API") << "Api " << name << " failed with TimeoutException " << std::string(e.what()) << LE;
        return createDtoResponse(Status::CODE_504, dto);
    } catch (NotFoundException & e){
        auto dto = MessageDto::createShared();
        dto->message = name + " not found: " + std::string(e.what());
        LWARNING("API") << "Api " << name << " failed with NotFoundException " << std::string(e.what()) << LE;
        return createDtoResponse(Status::CODE_404, dto);
    } catch (std::exception & e){
        auto dto = MessageDto::createShared();
        dto->message = "Failed to retrieve " + name + ": " + std::string(e.what());
        LWARNING("API") << "Api " << name << " failed with Exception " << std::string(e.what()) << LE;
        return createDtoResponse(Status::CODE_500, dto);
    }
}

template <class T>
T waitFor(std::future<T> future, unsigned int timeoutMs = 2000){
    auto status = future.wait_for(std::chrono::milliseconds(timeoutMs));
    if (status == std::future_status::ready){
        return future.get();
    } else if (status == std::future_status::timeout) {
        throw TimeoutException("Timeout waiting for future");
    } else if (status == std::future_status::deferred) {
        throw TimeoutException("Deferred future");
    } else {
        throw std::runtime_error("Unknown future status after wait");
    }
}

SMBRController::SMBRController(const std::shared_ptr<oatpp::web::mime::ContentMappers>& apiContentMappers,
                           std::shared_ptr<ISystemModule> systemModule)
    : oatpp::web::server::api::ApiController(apiContentMappers)
    , systemModule(systemModule)
    , activeCapturePromise(nullptr){
        scheduler_ = std::make_shared<Scheduler>(systemModule);
        recipes_ = std::make_shared<Recipes>("/data/recipes/");

    captureWorker = std::thread([this]() {
        while (true) {
            std::function<void()> task;

            {
                std::unique_lock<std::mutex> lock(queueMutex);
                //TODO timeout
                captureQueueCondition.wait(lock, [this]() { return stopCaptureWorker || !captureQueue.empty(); });

                if (stopCaptureWorker && captureQueue.empty()) break;

                task = std::move(captureQueue.front());
                captureQueue.pop();
            }

            task();
        }
    });
}

SMBRController::~SMBRController() {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        stopCaptureWorker = true;
        if (activeCapturePromise) {
            try {
                activeCapturePromise->set_exception(std::make_exception_ptr(std::runtime_error("Controller is being destroyed")));
            } catch (...) {
                std::cerr << "Exception caught while setting exception on activeCapturePromise" << std::endl;
            }
            activeCapturePromise.reset();
        }
    }
    captureQueueCondition.notify_all();
    if (captureWorker.joinable())
        captureWorker.join();
}

  // ==========================================
  // System Endpoints
  // ==========================================

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getSystemModules() {
    LDEBUG("API") << "Api getSystemModules begin" << LE;
    auto dtoList = oatpp::List<oatpp::Object<ModuleInfoDto>>::createShared();

    auto result = waitFor(systemModule->getAvailableModules(), 500);

    for (auto m : result){
        auto moduleInfoDto = ModuleInfoDto::createShared();

        if (m.type == Modules::Core) {
            moduleInfoDto->module_type = "core";
        } else if (m.type == Modules::Control) {
            moduleInfoDto->module_type = "control";
        } else if (m.type == Modules::Sensor) {
            moduleInfoDto->module_type = "sensor";
        } else {
            moduleInfoDto->module_type = "unknown";
        }

        moduleInfoDto->uid = m.uidHex;
        moduleInfoDto->instance = instanceToString(m.instance);
        dtoList->push_back(moduleInfoDto);
    }

    LDEBUG("API") << "Api getSystemModules end" << LE;
    return createDtoResponse(Status::CODE_200, dtoList);
}

std::string SMBRController::instanceToString(Instance instance) {
    switch (instance) {
        case Instance::Undefined: return "Undefined";
        case Instance::Exclusive: return "Exclusive";
        case Instance::All: return "All";
        case Instance::Reserved: return "Reserved";
        case Instance::Instance_1: return "Instance_1";
        case Instance::Instance_2: return "Instance_2";
        case Instance::Instance_3: return "Instance_3";
        case Instance::Instance_4: return "Instance_4";
        case Instance::Instance_5: return "Instance_5";
        case Instance::Instance_6: return "Instance_6";
        case Instance::Instance_7: return "Instance_7";
        case Instance::Instance_8: return "Instance_8";
        case Instance::Instance_9: return "Instance_9";
        case Instance::Instance_10: return "Instance_10";
        case Instance::Instance_11: return "Instance_11";
        case Instance::Instance_12: return "Instance_12";
        default: return "Unknown";
    }
}

  // ==========================================
  // Common Endpoints
  // ==========================================

std::shared_ptr<ICommonModule> SMBRController::getModule(const oatpp::Enum<dto::ModuleEnum>::AsString& module){
    if (module == dto::ModuleEnum::control) {
        return systemModule->commonModule(systemModule->controlModule()->id());
    } else if (module == dto::ModuleEnum::sensor) {
        return systemModule->commonModule(systemModule->sensorModule()->id());
    } else if (module == dto::ModuleEnum::core) {
        return systemModule->commonModule(systemModule->coreModule()->id());
    }
    throw NotFoundException("Module not found");
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::ping(const oatpp::Enum<dto::ModuleEnum>::AsString& module) {

    return process(__FUNCTION__, [&](){
        float responseTime = waitFor(getModule(module)->ping());
        auto pingResponseDto = PingResponseDto::createShared();
        pingResponseDto->time_ms = responseTime;
        return createDtoResponse(Status::CODE_200, pingResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getCoreLoad(const oatpp::Enum<dto::ModuleEnum>::AsString& module) {
    return process(__FUNCTION__, [&](){
        float load = waitFor(getModule(module)->getCoreLoad());
        auto loadResponseDto = LoadResponseDto::createShared();
        loadResponseDto->load = load;
        return createDtoResponse(Status::CODE_200, loadResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getCoreTemp(const oatpp::Enum<dto::ModuleEnum>::AsString& module) {
    return process(__FUNCTION__, [&](){
        float temperature = waitFor(getModule(module)->getCoreTemp());
        auto tempResponseDto = TempDto::createShared();
        tempResponseDto->temperature = temperature;
        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getBoardTemp(const oatpp::Enum<dto::ModuleEnum>::AsString& module) {
    return process(__FUNCTION__, [&](){
        float temperature = waitFor(getModule(module)->getBoardTemp());
        auto tempResponseDto = TempDto::createShared();
        tempResponseDto->temperature = temperature;
        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });
}

static ICommonModule::Ptr byUID(ISystemModule::Ptr s, const oatpp::data::type::EnumObjectWrapper<dto::ModuleEnum, oatpp::data::type::EnumInterpreterAsString<dto::ModuleEnum, false>>& module,
    const oatpp::Object<ModuleActionRequestDto>& body){
    if (!body || !body->uid) {
        throw NotFoundException("uid expected");
    }
    ModuleID moduleID;
    moduleID.uidHex = body->uid->c_str();


    if (module == dto::ModuleEnum::control) {
        moduleID.type == Modules::Control;
    } else if (module == dto::ModuleEnum::sensor) {
        moduleID.type == Modules::Sensor;
    } else if (module == dto::ModuleEnum::core) {
        moduleID.type == Modules::Core;
    }
    return s->commonModule(moduleID);
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::postRestart(
    const oatpp::data::type::EnumObjectWrapper<dto::ModuleEnum, oatpp::data::type::EnumInterpreterAsString<dto::ModuleEnum, false>>& module,
    const oatpp::Object<ModuleActionRequestDto>& body) {

    return processBool(__FUNCTION__, [&](){
        auto m = byUID(systemModule, module, body);
        return waitFor(m->restartModule());
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::postUsbBootloader(
    const oatpp::Enum<dto::ModuleEnum>::AsString& module,
    const oatpp::Object<ModuleActionRequestDto>& body) {

    return processBool(__FUNCTION__, [&](){
        auto m = byUID(systemModule, module, body);
        return waitFor(m->rebootModuleUsbBootloader());
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::postCanBootloader(
    const oatpp::Enum<dto::ModuleEnum>::AsString& module,
    const oatpp::Object<ModuleActionRequestDto>& body) {

    return processBool(__FUNCTION__, [&](){
        auto m = byUID(systemModule, module, body);
        return waitFor(m->rebootModuleCanBootloader());
    });
}

// ==========================================
// Core module
// ==========================================

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getShortID() {

    return process(__FUNCTION__, [&](){
        auto sid = waitFor(systemModule->coreModule()->getShortID());
        auto sidResponseDto = SIDDto::createShared();
        sidResponseDto->sid = sid;
        return createDtoResponse(Status::CODE_200, sidResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getIpAddress() {

    return process(__FUNCTION__, [&](){
        auto ipAddress = waitFor(systemModule->coreModule()->getIpAddress());
        auto ipResponseDto = IpDto::createShared();
        ipResponseDto->ipAddress = ipAddress;
        return createDtoResponse(Status::CODE_200, ipResponseDto);
    });
}


std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getHostname() {

    return process(__FUNCTION__, [&](){
        auto hostname = waitFor(systemModule->coreModule()->getHostname());
        auto hostnameResponseDto = HostnameDto::createShared();
        hostnameResponseDto->hostname = hostname;
        return createDtoResponse(Status::CODE_200, hostnameResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getSerialNumber() {

    return process(__FUNCTION__, [&](){
        auto serial = waitFor(systemModule->coreModule()->getSerialNumber());
        auto serialResponseDto = SerialDto::createShared();
        serialResponseDto->serial = serial;
        return createDtoResponse(Status::CODE_200, serialResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getPowerSupplyType() {

    return process(__FUNCTION__, [&](){
        auto res = waitFor(systemModule->coreModule()->getPowerSupplyType());

        auto supplyTypeResponseDto = SupplyTypeDto::createShared();
        supplyTypeResponseDto->vin = res.isVIN;
        supplyTypeResponseDto->poe = res.isPoE;
        supplyTypeResponseDto->poe_hb = res.isPoE_Hb;
        return createDtoResponse(Status::CODE_200, supplyTypeResponseDto);

    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getVoltage5V() {

    return process(__FUNCTION__, [&](){
        auto voltage = waitFor(systemModule->coreModule()->getVoltage5V());
        auto voltageResponseDto = VoltageDto::createShared();
        voltageResponseDto->voltage = voltage;
        return createDtoResponse(Status::CODE_200, voltageResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getVoltageVIN() {

    return process(__FUNCTION__, [&](){
        auto voltage = waitFor(systemModule->coreModule()->getVoltageVIN());
        auto voltageResponseDto = VoltageDto::createShared();
        voltageResponseDto->voltage = voltage;
        return createDtoResponse(Status::CODE_200, voltageResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getPoEVoltage() {

    return process(__FUNCTION__, [&](){
        auto voltage = waitFor(systemModule->coreModule()->getVoltagePoE());
        auto voltageResponseDto = VoltageDto::createShared();
        voltageResponseDto->voltage = voltage;
        return createDtoResponse(Status::CODE_200, voltageResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getCurrentConsumption() {

    return process(__FUNCTION__, [&](){
        auto current = waitFor(systemModule->coreModule()->getCurrentConsumption());
        auto currentResponseDto = CurrentDto::createShared();
        currentResponseDto->current = current;
        return createDtoResponse(Status::CODE_200, currentResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getPowerDraw() {

    return process(__FUNCTION__, [&](){
        auto powerDraw = waitFor(systemModule->coreModule()->getPowerDraw());
        auto powerDrawResponseDto = PowerDrawDto::createShared();
        powerDrawResponseDto->power_draw = powerDraw;
        return createDtoResponse(Status::CODE_200, powerDrawResponseDto);
    });
}

// ==========================================
// Control module
// ==========================================
std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::setIntensities(const oatpp::Object<IntensitiesDto>& body) {

    return processBool(__FUNCTION__, [&](){
        if (!body || !body->intensity || body->intensity->size() != 4) {
            throw ArgumentException("Invalid intensity array. Must contain exactly 4 values.");
        }

        float ii0, ii1, ii2, ii3;

        auto processIntensity = [](float & i, const oatpp::Float32 & intensity){
            if (!intensity){
                throw ArgumentException("Invalid intensity value. Must be between 0.0 and 1.0.");
            }
            if (*intensity < 0.0f || *intensity > 1.0f) {
                throw ArgumentException("Invalid intensity value. Must be between 0.0 and 1.0.");
            }
            i = *intensity;
        };

        processIntensity(ii0, body->intensity->at(0));
        processIntensity(ii1, body->intensity->at(1));
        processIntensity(ii2, body->intensity->at(2));
        processIntensity(ii3, body->intensity->at(3));

        return waitFor(systemModule->controlModule()->setIntensities(ii0, ii1, ii2, ii3));
    });
}

int SMBRController::getChannel(const dto::ChannelEnum& channel) {
    switch (channel) {
        case dto::ChannelEnum::channel0:
            return 0;
        case dto::ChannelEnum::channel1:
            return 1;
        case dto::ChannelEnum::channel2:
            return 2;
        case dto::ChannelEnum::channel3:
            return 3;
        default:
            throw NotFoundException("Channel not found");
    }
}


std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::setIntensity(
    const oatpp::Enum<dto::ChannelEnum>::AsString& channel,
    const oatpp::Object<IntensityDto>& body
) {

    return processBool(__FUNCTION__, [&](){
        if (body->intensity < 0 || body->intensity > 1) {
            throw ArgumentException("Invalid intensity. Must be between 0 and 1.");
        }

        return waitFor(systemModule->controlModule()->setIntensity(body->intensity, getChannel(channel)));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getIntensity(const oatpp::Enum<dto::ChannelEnum>::AsString& channel) {

    return process(__FUNCTION__, [&](){
        auto response = waitFor(systemModule->controlModule()->getIntensity(getChannel(channel)));
        auto intensityResponseDto = IntensityDto::createShared();
        intensityResponseDto->intensity = response.intensity;
        return createDtoResponse(Status::CODE_200, intensityResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getLedTemperature() {

    return process(__FUNCTION__, [&](){
        auto temperature = waitFor(systemModule->controlModule()->getLedTemperature());
        auto tempResponseDto = TempDto::createShared();
        tempResponseDto->temperature = temperature;
        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });
}


std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::setHeaterIntensity(const oatpp::Object<IntensityDto>& body) {

    return processBool(__FUNCTION__, [&](){
        if (!body || body->intensity < -1.0f || body->intensity > 1.0f) {
            throw ArgumentException("Invalid intensity value. Must be between -1.0 and 1.0.");
        }

        return waitFor(systemModule->controlModule()->setHeaterIntensity(body->intensity));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getHeaterIntensity() {

    return process(__FUNCTION__, [&](){
        auto intensity = waitFor(systemModule->controlModule()->getHeaterIntensity());
        auto intensityResponseDto = IntensityDto::createShared();
        intensityResponseDto->intensity = intensity;
        return createDtoResponse(Status::CODE_200, intensityResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::setHeaterTargetTemperature(const oatpp::Object<TempDto>& body) {

    return processBool(__FUNCTION__, [&](){
        if (!body || body->temperature < 0.0f) {
            throw ArgumentException("Invalid target temperature. Must be a positive value.");
        }

        return waitFor(systemModule->controlModule()->setHeaterTargetTemperature(body->temperature));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getHeaterTargetTemperature() {
    return process(__FUNCTION__, [&](){
        auto temperature = waitFor(systemModule->controlModule()->getHeaterTargetTemperature());

        auto tempResponseDto = TempDto::createShared();

        if (std::isnan(temperature)) {
            auto tempNullDto = TempNullDto::createShared();
            tempNullDto->temperature = nullptr;
            return createDtoResponse(Status::CODE_200, tempNullDto);
        }

        tempResponseDto->temperature = oatpp::Float32(temperature);

        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getHeaterPlateTemperature() {

    return process(__FUNCTION__, [&](){
        auto plateTemperature = waitFor(systemModule->controlModule()->getHeaterPlateTemperature());
        auto plateTempResponseDto = TempDto::createShared();
        plateTempResponseDto->temperature = plateTemperature;
        return createDtoResponse(Status::CODE_200, plateTempResponseDto);
    });
}


std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::turnOffHeater() {

    return processBool(__FUNCTION__, [&](){
        return waitFor(systemModule->controlModule()->turnOffHeater());
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::setCuvettePumpSpeed(const oatpp::Object<SpeedDto>& body) {

    return processBool(__FUNCTION__, [&](){
        if (!body || body->speed < -1.0f || body->speed > 1.0f) {
            throw ArgumentException("Invalid speed value. Must be between -1.0 and 1.0.");
        }

        return waitFor(systemModule->controlModule()->setCuvettePumpSpeed(body->speed));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getCuvettePumpSpeed() {

    return process(__FUNCTION__, [&](){
        auto speed = waitFor(systemModule->controlModule()->getCuvettePumpSpeed());
        auto speedResponseDto = SpeedDto::createShared();
        speedResponseDto->speed = speed;
        return createDtoResponse(Status::CODE_200, speedResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::setCuvettePumpFlowrate(const oatpp::Object<FlowrateDto>& body) {

    return processBool(__FUNCTION__, [&](){
        if (!body || body->flowrate < -1000.0f || body->flowrate > 1000.0f) {
            throw ArgumentException("Invalid flowrate value. Must be between -1000.0 and 1000.0.");
        }

        return waitFor(systemModule->controlModule()->setCuvettePumpFlowrate(body->flowrate));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getCuvettePumpFlowrate() {

    return process(__FUNCTION__, [&](){
        auto flowrate = waitFor(systemModule->controlModule()->getCuvettePumpFlowrate());
        auto flowrateResponseDto = FlowrateDto::createShared();
        flowrateResponseDto->flowrate = flowrate;
        return createDtoResponse(Status::CODE_200, flowrateResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::moveCuvettePump(const oatpp::Object<MoveDto>& body) {

    return processBool(__FUNCTION__, [&](){
        if (!body || body->volume < 0.0f || body->volume > 1000.0f) {
            throw ArgumentException("Invalid volume value. Must be between 0.0 and 1000.0.");
        }
        if (!body || body->flowrate < -1000.0f || body->flowrate > 1000.0f) {
            throw ArgumentException("Invalid flowrate value. Must be between -1000.0 and 1000.0.");
        }
        return waitFor(systemModule->controlModule()->moveCuvettePump(body->volume, body->flowrate));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::primeCuvettePump() {

    return processBool(__FUNCTION__, [&](){
        return waitFor(systemModule->controlModule()->primeCuvettePump());
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::purgeCuvettePump() {

    return processBool(__FUNCTION__, [&](){
        return waitFor(systemModule->controlModule()->purgeCuvettePump());
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::stopCuvettePump() {

    return processBool(__FUNCTION__, [&](){
        return waitFor(systemModule->controlModule()->stopCuvettePump());
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::setAeratorSpeed(const oatpp::Object<SpeedDto>& body) {

    return processBool(__FUNCTION__, [&](){
        if (!body || body->speed < 0.0f || body->speed > 1.0f) {
            throw ArgumentException("Invalid speed value. Must be between 0.0 and 1.0.");
        }

        return waitFor(systemModule->controlModule()->setAeratorSpeed(body->speed));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getAeratorSpeed() {

    return process(__FUNCTION__, [&](){
        auto speed = waitFor(systemModule->controlModule()->getAeratorSpeed());
        auto speedResponseDto = SpeedDto::createShared();
        speedResponseDto->speed = speed;
        return createDtoResponse(Status::CODE_200, speedResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::setAeratorFlowrate(const oatpp::Object<FlowrateDto>& body) {

    return processBool(__FUNCTION__, [&](){
        if (!body || body->flowrate < 10.0f || body->flowrate > 5000.0f) {
            throw ArgumentException("Invalid flowrate value. Must be between 10.0 and 5000.0 ml/min.");
        }

        return waitFor(systemModule->controlModule()->setAeratorFlowrate(body->flowrate));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getAeratorFlowrate() {

    return process(__FUNCTION__, [&](){
        auto flowrate = waitFor(systemModule->controlModule()->getAeratorFlowrate());
        auto flowrateResponseDto = FlowrateDto::createShared();
        flowrateResponseDto->flowrate = flowrate;
        return createDtoResponse(Status::CODE_200, flowrateResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::moveAerator(const oatpp::Object<MoveDto>& body) {

    return processBool(__FUNCTION__, [&](){
        if (!body || body->volume < 0.0f || body->volume > 1000.0f) {
            throw ArgumentException("Invalid volume value. Must be between 0.0 and 1000.0.");
        }
        if (!body || body->flowrate < 10.0f || body->flowrate > 5000.0f) {
            throw ArgumentException("Invalid flowrate value. Must be between 10.0 and 5000.0 ml/min.");
        }

        return waitFor(systemModule->controlModule()->moveAerator(body->volume, body->flowrate));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::stopAerator() {

    return processBool(__FUNCTION__, [&](){
        return waitFor(systemModule->controlModule()->stopAerator());
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::setMixerSpeed(const oatpp::Object<SpeedDto>& body) {

    return processBool(__FUNCTION__, [&](){
        if (!body || body->speed < 0.0f || body->speed > 1.0f) {
            throw ArgumentException("Invalid speed value. Must be between 0.0 and 1.0.");
        }

        return waitFor(systemModule->controlModule()->setMixerSpeed(body->speed));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getMixerSpeed() {

    return process(__FUNCTION__, [&](){
        auto speed = waitFor(systemModule->controlModule()->getMixerSpeed());
        auto speedResponseDto = SpeedDto::createShared();
        speedResponseDto->speed = speed;
        return createDtoResponse(Status::CODE_200, speedResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::setMixerRpm(const oatpp::Object<RpmDto>& body) {

    return processBool(__FUNCTION__, [&](){
        if (!body || body->rpm < 0.0f || body->rpm > 10000.0f) {
            throw ArgumentException("Invalid RPM value. Must be between 0 and 10000.");
        }

        return waitFor(systemModule->controlModule()->setMixerRpm(body->rpm));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getMixerRpm() {

    return process(__FUNCTION__, [&](){
        auto rpm = waitFor(systemModule->controlModule()->getMixerRpm());
        auto rpmResponseDto = RpmDto::createShared();
        rpmResponseDto->rpm = rpm;
        return createDtoResponse(Status::CODE_200, rpmResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::stirMixer(const oatpp::Object<StirDto>& body) {

    return processBool(__FUNCTION__, [&](){
        if (!body || body->rpm < 0.0f || body->rpm > 10000.0f || body->time < 0.0f || body->time > 3600.0f) {
            throw ArgumentException("Invalid RPM or time value. RPM must be between 0 and 10000, and time must be between 0 and 3600 seconds.");
        }

        return waitFor(systemModule->controlModule()->stirMixer(body->rpm, body->time));
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::stopMixer() {

    return processBool(__FUNCTION__, [&](){
        return waitFor(systemModule->controlModule()->stopMixer());
    });
}

// ==========================================
// Sensor module
// ==========================================

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getBottleTemperature() {

    return process(__FUNCTION__, [&](){
        auto temperature = waitFor(systemModule->sensorModule()->getBottleTemperature());
        auto tempResponseDto = TempDto::createShared();
        tempResponseDto->temperature = temperature;
        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getTopMeasuredTemperature() {

    return process(__FUNCTION__, [&](){
        auto temperature = waitFor(systemModule->sensorModule()->getTopMeasuredTemperature());
        auto tempResponseDto = TempDto::createShared();
        tempResponseDto->temperature = temperature;
        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getBottomMeasuredTemperature() {

    return process(__FUNCTION__, [&](){
        auto temperature = waitFor(systemModule->sensorModule()->getBottomMeasuredTemperature());
        auto tempResponseDto = TempDto::createShared();
        tempResponseDto->temperature = temperature;
        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getTopSensorTemperature() {

    return process(__FUNCTION__, [&](){
        auto temperature = waitFor(systemModule->sensorModule()->getTopSensorTemperature());
        auto tempResponseDto = TempDto::createShared();
        tempResponseDto->temperature = temperature;
        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getBottomSensorTemperature() {

    return process(__FUNCTION__, [&](){
        auto temperature = waitFor(systemModule->sensorModule()->getBottomSensorTemperature());
        auto tempResponseDto = TempDto::createShared();
        tempResponseDto->temperature = temperature;
        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::clearCustomText() {
    return processBool(__FUNCTION__, [&](){
        return waitFor(systemModule->sensorModule()->clearCustomText());
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::printCustomText(const oatpp::Object<TextDto>& body) {
    return processBool(__FUNCTION__, [&](){
        return waitFor(systemModule->sensorModule()->printCustomText(body->text));
    });
}

Fluorometer_config::Gain SMBRController::getGain(const dto::GainEnum& gain) {
    switch (gain) {
        case dto::GainEnum::x1:
            return Fluorometer_config::Gain::x1;
        case dto::GainEnum::x10:
            return Fluorometer_config::Gain::x10;
        case dto::GainEnum::x50:
            return Fluorometer_config::Gain::x50;
        case dto::GainEnum::Auto:
            return Fluorometer_config::Gain::Auto;
        default:
            throw NotFoundException("Gain not found");
    }
}

Fluorometer_config::Timing SMBRController::getTiming(const dto::TimingEnum& timing) {
    switch (timing) {
        case dto::TimingEnum::Linear:
            return Fluorometer_config::Timing::Linear;
        case dto::TimingEnum::Logarithmic:
            return Fluorometer_config::Timing::Logarithmic;
        default:
            throw NotFoundException("Timing not found");
    }
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::performFluorometerSingleSample(
    const oatpp::Enum<dto::GainEnum>::AsString& gain,
    const oatpp::Object<FluorometerSingleSampleRequestDto>& body
) {
    return process(__FUNCTION__, [&]() {
        if (body->emitor_intensity < 0.2f || body->emitor_intensity > 1.0f) {
            throw ArgumentException("Invalid emitor intensity. Must be between 0.2 and 1.0");
        }

        Fluorometer_config::Gain detector_gain = getGain(gain);

        auto sample = waitFor(systemModule->sensorModule()->takeFluorometerSingleSample(detector_gain, body->emitor_intensity));

        auto responseDto = FluorometerSingleSampleResponseDto::createShared();
        responseDto->raw_value = sample.raw_value;
        responseDto->relative_value = sample.relative_value;
        responseDto->absolute_value = sample.absolute_value;

        return createDtoResponse(Status::CODE_200, responseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::captureFluorometerOjip(
    const oatpp::Enum<dto::GainEnum>::AsString& gain,
    const oatpp::Enum<dto::TimingEnum>::AsString& timing,
    const oatpp::String& length_ms,
    const oatpp::String& sample_count,
    const oatpp::Object<FluorometerOjipCaptureRequestDto>& body
) {
    oatpp::UInt16 finalLengthMs;
    try {
        finalLengthMs = length_ms->empty() ? body->length_ms : static_cast<oatpp::UInt16>(std::stoi(length_ms->c_str()));
    } catch (const std::exception& e) {
        auto dto = MessageDto::createShared();
        dto->message = "Invalid value for length_ms.";
        return createDtoResponse(Status::CODE_500, dto);
    }

    if (finalLengthMs < 200 || finalLengthMs > 4000) {
        auto dto = MessageDto::createShared();
        dto->message = "Invalid sample count. Must be between 200 and 4000.";
        return createDtoResponse(Status::CODE_500, dto);
    }

    oatpp::UInt16 finalSampleCount;
    try {
        finalSampleCount = sample_count->empty() ? body->sample_count : static_cast<oatpp::UInt16>(std::stoi(sample_count->c_str()));
    } catch (const std::exception& e) {
        auto dto = MessageDto::createShared();
        dto->message = "Invalid value for sample_count.";
        return createDtoResponse(Status::CODE_500, dto);
    }

    if (finalSampleCount < 200 || finalSampleCount > 4000) {
        auto dto = MessageDto::createShared();
        dto->message = "Invalid sample count. Must be between 200 and 4000.";
        return createDtoResponse(Status::CODE_500, dto);
    }

    {
        std::unique_lock<std::mutex> lock(retrieveMutex);
        //TODO timeout
        retrieveCondition.wait(lock, [this]() {
            return !retrievingInProgress;
        });
    }

    std::unique_lock<std::mutex> captureLock(captureMutex);

    auto promise = std::make_shared<std::promise<std::shared_ptr<oatpp::web::protocol::http::outgoing::Response>>>();
    auto future = std::make_shared<std::shared_future<std::shared_ptr<oatpp::web::protocol::http::outgoing::Response>>>(promise->get_future().share());

    {
        std::lock_guard<std::mutex> lock(activeCaptureMutex);
        activeCaptureFuture = future;
    }

    {
        std::lock_guard<std::mutex> lock(queueMutex);

        captureQueue.push([this, promise, gain, timing, finalLengthMs, finalSampleCount, body]() {

            try {
                ISensorModule::FluorometerInput input{
                    .detector_gain = getGain(gain),
                    .sample_timing = getTiming(timing),
                    .emitor_intensity = body->emitor_intensity,
                    .length_ms = finalLengthMs,
                    .sample_count = finalSampleCount
                };

                //TODO specify timeout
                auto fluorometerData = waitFor(systemModule->sensorModule()->captureFluorometerOjip(input), finalLengthMs + 30000);

                auto ojipDataDto = FluorometerMeasurementDto::createShared();
                ojipDataDto->measurement_id = fluorometerData.measurement_id;
                ojipDataDto->detector_gain = static_cast<dto::GainEnum>(fluorometerData.detector_gain);
                ojipDataDto->timebase = static_cast<dto::TimingEnum>(fluorometerData.timebase);
                ojipDataDto->emitor_intensity = fluorometerData.emitor_intensity;
                ojipDataDto->length_ms = fluorometerData.length_ms;
                ojipDataDto->required_samples = fluorometerData.required_samples;
                ojipDataDto->captured_samples = fluorometerData.captured_samples;
                ojipDataDto->missing_samples = fluorometerData.missing_samples;
                ojipDataDto->read = fluorometerData.read;
                ojipDataDto->saturated = fluorometerData.saturated;

                oatpp::Vector<oatpp::Object<FluorometerSampleDto>> samples = oatpp::Vector<oatpp::Object<FluorometerSampleDto>>::createShared();
                for (const auto& sample : fluorometerData.samples) {
                    auto sampleDto = FluorometerSampleDto::createShared();
                    sampleDto->time_ms = sample.time_ms;
                    sampleDto->raw_value = sample.raw_value;
                    sampleDto->relative_value = sample.relative_value;
                    sampleDto->absolute_value = sample.absolute_value;
                    samples->push_back(sampleDto);
                }
                ojipDataDto->samples = samples;

                promise->set_value(createDtoResponse(Status::CODE_200, ojipDataDto));
            } catch (const std::exception& e) {
                auto dto = MessageDto::createShared();
                dto->message = "Error during fluorometer capture: " + std::string(e.what());
                promise->set_value(createDtoResponse(Status::CODE_504, dto));
            }

            std::lock_guard<std::mutex> lock(activeCaptureMutex);
            activeCaptureFuture.reset();
        });
    }

    captureQueueCondition.notify_all();
    return future->get();
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::checkFluorometerOjipCaptureComplete() {
    return process(__FUNCTION__, [&](){
        //TODO specify timeout
        auto captureComplete = waitFor(systemModule->sensorModule()->isFluorometerOjipCaptureComplete(), 30000);
        auto captureStatusDto = FluorometerCaptureStatusDto::createShared();
        captureStatusDto->capture_complete = captureComplete;
        return createDtoResponse(Status::CODE_200, captureStatusDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::retrieveLastFluorometerOjipData() {

    {
        std::lock_guard<std::mutex> lock(activeCaptureMutex);
        if (activeCaptureFuture) {
            try {
                return activeCaptureFuture->get();
            } catch (const std::exception& e) {
                return createResponse(Status::CODE_500, e.what());
            }
        }
    }

    std::unique_lock<std::mutex> lock(retrieveMutex);

    if (retrievingInProgress) {
        //TODO timeout
        retrieveCondition.wait(lock, [this]() {
            return !retrievingInProgress;
        });

        return cachedResponse;
    }

    retrievingInProgress = true;
    cachedResponse = nullptr;
    lock.unlock();

    auto response = process(__FUNCTION__, [&]() {
        //TODO timeout
        auto fluorometerData = waitFor(systemModule->sensorModule()->retrieveLastFluorometerOjipData(), 30000);

        auto dto = FluorometerMeasurementDto::createShared();
        dto->measurement_id = fluorometerData.measurement_id;
        dto->detector_gain = static_cast<dto::GainEnum>(fluorometerData.detector_gain);
        dto->timebase = static_cast<dto::TimingEnum>(fluorometerData.timebase);
        dto->emitor_intensity = fluorometerData.emitor_intensity;
        dto->length_ms = fluorometerData.length_ms;
        dto->required_samples = fluorometerData.required_samples;
        dto->captured_samples = fluorometerData.captured_samples;
        dto->missing_samples = fluorometerData.missing_samples;
        dto->read = fluorometerData.read;
        dto->saturated = fluorometerData.saturated;

        oatpp::Vector<oatpp::Object<FluorometerSampleDto>> samples = oatpp::Vector<oatpp::Object<FluorometerSampleDto>>::createShared();
        for (const auto& sample : fluorometerData.samples) {
            auto sampleDto = FluorometerSampleDto::createShared();
            sampleDto->time_ms = sample.time_ms;
            sampleDto->raw_value = sample.raw_value;
            sampleDto->relative_value = sample.relative_value;
            sampleDto->absolute_value = sample.absolute_value;
            samples->push_back(sampleDto);
        }
        dto->samples = samples;

        return createDtoResponse(Status::CODE_200, dto);
    });

    lock.lock();
    cachedResponse = response;
    retrievingInProgress = false;
    retrieveCondition.notify_all();
    return response;
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getFluorometerDetectorInfo() {
    return process(__FUNCTION__, [&](){
        auto info = waitFor(systemModule->sensorModule()->getFluorometerDetectorInfo());
        auto infoDto = FluorometerDetectorInfoDto::createShared();
        std::cout<<info.peak_wavelength<<" "<<info.sensitivity<<" "<<info.sampling_rate<<std::endl;
        infoDto->peak_wavelength = info.peak_wavelength;
        infoDto->sensitivity = info.sensitivity;
        infoDto->sampling_rate = info.sampling_rate;
        return createDtoResponse(Status::CODE_200, infoDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getFluorometerDetectorTemperature() {
    return process(__FUNCTION__, [&](){
        auto temperature = waitFor(systemModule->sensorModule()->getFluorometerDetectorTemperature());
        auto tempResponseDto = TempDto::createShared();
        tempResponseDto->temperature = temperature;
        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getFluorometerEmitorInfo() {
    return process(__FUNCTION__, [&](){
        auto info = waitFor(systemModule->sensorModule()->getFluorometerEmitorInfo());
        auto infoDto = FluorometerEmitorInfoDto::createShared();
        infoDto->peak_wavelength = info.peak_wavelength;
        infoDto->power_output = info.power_output;
        return createDtoResponse(Status::CODE_200, infoDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getFluorometerEmitorTemperature() {
    return process(__FUNCTION__, [&](){
        auto temperature = waitFor(systemModule->sensorModule()->getFluorometerEmitorTemperature());
        auto tempResponseDto = TempDto::createShared();
        tempResponseDto->temperature = temperature;
        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getSpectrophotometerChannels() {
    return process(__FUNCTION__, [&](){
        auto channels = waitFor(systemModule->sensorModule()->getSpectrophotometerChannels());
        auto responseDto = SpectroChannelsDto::createShared();
        responseDto->channels = channels;
        return createDtoResponse(Status::CODE_200, responseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getSpectrophotometerChannelInfo(const oatpp::UInt8& channel) {
    return process(__FUNCTION__, [&](){
        auto channelInfo = waitFor(systemModule->sensorModule()->getSpectrophotometerChannelInfo(channel));
        auto responseDto = SpectroChannelInfoDto::createShared();
        responseDto->channel = channelInfo.channel;
        responseDto->peak_wavelength = channelInfo.peak_wavelength;
        responseDto->half_intensity_peak_width = channelInfo.half_intensity_peak_width;
        return createDtoResponse(Status::CODE_200, responseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::measureAllSpectrophotometerChannels() {
    return process(__FUNCTION__, [&](){
        auto channelCount = waitFor(systemModule->sensorModule()->getSpectrophotometerChannels());

        auto measurementsDto = SpectroMeasurementsDto::createShared();
        measurementsDto->samples = oatpp::Vector<oatpp::Object<SingleChannelMeasurementDto>>::createShared();

        for (int8_t channel = 0; channel < channelCount; channel++) {
            auto measurement = waitFor(systemModule->sensorModule()->measureSpectrophotometerChannel(channel));
            auto channelMeasurement = SingleChannelMeasurementDto::createShared();
            channelMeasurement->channel = measurement.channel;
            channelMeasurement->relative_value = measurement.relative_value;
            channelMeasurement->absolute_value = measurement.absolute_value;
            measurementsDto->samples->push_back(channelMeasurement);
        }

        return createDtoResponse(Status::CODE_200, measurementsDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::measureSingleSpectrophotometerChannel(const Int8& channel) {
    return process(__FUNCTION__, [&](){
        auto measurement = waitFor(systemModule->sensorModule()->measureSpectrophotometerChannel(channel));
        auto responseDto = SingleChannelMeasurementDto::createShared();
        responseDto->channel = measurement.channel;
        responseDto->relative_value = measurement.relative_value;
        responseDto->absolute_value = measurement.absolute_value;
        return createDtoResponse(Status::CODE_200, responseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getSpectrophotometerEmitorTemperature() {
    return process(__FUNCTION__, [&](){
        auto temperature = waitFor(systemModule->sensorModule()->getSpectrophotometerEmitorTemperature());
        auto tempResponseDto = TempDto::createShared();
        tempResponseDto->temperature = temperature;
        return createDtoResponse(Status::CODE_200, tempResponseDto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::calibrateSpectrophotometer(const oatpp::String& body)  {
    if (body != "{}") {
        auto dto = MessageDto::createShared();
        dto->message = "Invalid input";
        return createDtoResponse(Status::CODE_500, dto);
    }
    return processBool(__FUNCTION__, [&](){
        return waitFor(systemModule->sensorModule()->calibrateSpectrophotometer());
    });
}

// ==========================================
// Recipes
// ==========================================

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getRecipeList() {
    try {
        LDEBUG("API") << "Api getRecipeList begin" << LE;
        auto sc = recipes_->getRecipeNames();
        auto dtoList = oatpp::List<oatpp::String>::createShared();
        for (auto s : sc){
            dtoList->push_back(s);
        }
        LDEBUG("API") << "Api getRecipeList end (success)" << LE;
        return createDtoResponse(Status::CODE_200, dtoList);
    } catch (std::exception & e){
        LWARNING("API") << "Api getRecipeList end (failure: " << e.what() << ")" << LE;
        return createResponse(Status::CODE_500, "Failed to retrieve recipe list: " + std::string(e.what()));
    }
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::reloadRecipeList() {
    try {
        recipes_->reload();
        return getRecipeList();
    } catch (std::exception & e){
        return createResponse(Status::CODE_500, "Failed to reload recipe: " + std::string(e.what()));
    }
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getRecipeContent(const oatpp::String& name) {
    try {
        LDEBUG("API") << "Api getRecipeContent begin" << LE;
        auto s = recipes_->getRecipeContent(name);
        auto scriptResponseDto = ScriptDto::createShared();
        scriptResponseDto->name = s.name;
        scriptResponseDto->content = s.content;
        LDEBUG("API") << "Api getRecipeContent end (success)" << LE;
        return createDtoResponse(Status::CODE_200, scriptResponseDto);
    } catch (std::exception & e){
        LWARNING("API") << "Api getRecipeContent end (failure: " << e.what() << ")" << LE;
        return createResponse(Status::CODE_500, "Failed to retrieve recipe: " + std::string(e.what()));
    }
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::updateRecipe(const oatpp::String& name, const oatpp::Object<ScriptDto>& body) {
    try {
        LDEBUG("API") << "Api updateRecipe begin" << LE;
        if (!body || !body->name || !body->content) {
            throw ArgumentException("Invalid script. Must contain a name and content.");
        }
        ScriptInfo s;
        s.name = *body->name;
        s.content = *body->content;
        recipes_->replaceRecipe(s);
        LDEBUG("API") << "Api updateRecipe end (success)" << LE;
        return createResponse(Status::CODE_200, "Recipe updated successfully.");
    } catch (std::exception & e){
        LWARNING("API") << "Api updateRecipe end (failure: " << e.what() << ")" << LE;
        return createResponse(Status::CODE_500, "Failed to update recipe: " + std::string(e.what()));
    }
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::deleteRecipe(const oatpp::String& name) {
    try {
        LDEBUG("API") << "Api deleteRecipe begin" << LE;
        recipes_->deleteRecipe(name);
        LDEBUG("API") << "Api deleteRecipe end (success)" << LE;
        return createResponse(Status::CODE_200, "Recipe deleted successfully.");
    } catch (std::exception & e){
        LWARNING("API") << "Api deleteRecipe end (failure: " << e.what() << ")" << LE;
        return createResponse(Status::CODE_500, "Failed to delete recipe: " + std::string(e.what()));
    }
}


// ==========================================
// Scheduler
// ==========================================
std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::selectRecipe(const oatpp::String& name) {
    try {
        LDEBUG("API") << "Api selectRecipe begin" << LE;
        auto sc = recipes_->getRecipeContent(name);
        scheduler_->setScriptFromString(sc);
        LDEBUG("API") << "Api selectRecipe end (success)" << LE;
        return createResponse(Status::CODE_200, "Recipe " + name + " selected.");
    } catch (std::exception & e){
        LWARNING("API") << "Api selectRecipe end (failure: " << e.what() << ")" << LE;
        return createResponse(Status::CODE_500, "Failed to select script: " + std::string(e.what()));
    }
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getRecipe() {
    try {
        LDEBUG("API") << "Api getRecipe begin" << LE;
        auto s = scheduler_->getScript();
        auto scriptResponseDto = ScriptDto::createShared();
        scriptResponseDto->name = s.name;
        scriptResponseDto->content = s.content;
        LDEBUG("API") << "Api getRecipe end (success)" << LE;
        return createDtoResponse(Status::CODE_200, scriptResponseDto);
    } catch (std::exception & e){
        LWARNING("API") << "Api getRecipe end (failure: " << e.what() << ")" << LE;
        return createResponse(Status::CODE_500, "Failed to retrieve recipe: " + std::string(e.what()));
    }
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::startScheduler() {
    try {
        LDEBUG("API") << "Api startScheduler begin" << LE;
        auto processId = scheduler_->start();
        auto scriptProcessIdDto = ScriptProcessIdDto::createShared();
        scriptProcessIdDto->processId = processId;
        LDEBUG("API") << "Api startScheduler end (success)" << LE;
        return createDtoResponse(Status::CODE_200, scriptProcessIdDto);
    } catch (std::exception & e){
        LWARNING("API") << "Api startScheduler end (failure: " << e.what() << ")" << LE;
        return createResponse(Status::CODE_500, "Failed to start script: " + std::string(e.what()));
    }
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::stopScheduler() {
    try {
        LDEBUG("API") << "Api stopScheduler begin" << LE;
        scheduler_->stop();
        LDEBUG("API") << "Api stopScheduler end (success)" << LE;
        return createResponse(Status::CODE_200, "Script stopped successfully.");
    } catch (std::exception & e){
        LWARNING("API") << "Api stopScheduler end (failure: " << e.what() << ")" << LE;
        return createResponse(Status::CODE_500, "Failed to stop script: " + std::string(e.what()));
    }
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> SMBRController::getSchedulerInfo() {
    try {
        LDEBUG("API") << "Api getSchedulerInfo begin" << LE;
        RuntimeInfo info = scheduler_->getRuntimeInfo();
        auto infoResponseDto = ScriptRuntimeInfoDto::createShared();

        infoResponseDto->processId = info.processId;
        infoResponseDto->name = info.name;
        infoResponseDto->finalMessage = info.finishMessage;
        infoResponseDto->stack = oatpp::Vector<Int32>::createShared();
        for (auto i : info.stack){
            infoResponseDto->stack->push_back(i);
        }
        infoResponseDto->output = oatpp::Vector<String>::createShared();
        for (auto o : info.output){
            std::stringstream s;
            s << o;
            infoResponseDto->output->push_back(s.str());
        }
        infoResponseDto->started = info.started;
        infoResponseDto->stopped = info.stopped;

        infoResponseDto->startedAt = Poco::DateTimeFormatter::format(info.startTime, "%Y-%m-%d %H:%M:%S");;

        LDEBUG("API") << "Api getSchedulerInfo end" << LE;
        return createDtoResponse(Status::CODE_200, infoResponseDto);

    } catch (std::exception & e){
        LWARNING("API") << "Api getSchedulerInfo end (failure: " << e.what() << ")" << LE;
        return createResponse(Status::CODE_500, "Failed to retrieve scheduler status: " + std::string(e.what()));
    }
}

