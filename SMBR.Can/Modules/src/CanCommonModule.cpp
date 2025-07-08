#include "CanCommonModule.hpp"

#include "codes/codes.hpp"
#include <iostream>       // std::cout
#include <future>         // std::async, std::future

#include "codes/messages/common/ping_request.hpp"
#include "codes/messages/common/ping_response.hpp"
#include "codes/messages/common/core_temp_request.hpp"
#include "codes/messages/common/core_temp_response.hpp"
#include "codes/messages/common/device_reset.hpp"
#include "codes/messages/common/device_usb_bootloader.hpp"
#include "codes/messages/common/device_can_bootloader.hpp"
#include "codes/messages/common/core_load_request.hpp"
#include "codes/messages/common/core_load_response.hpp"
#include "codes/messages/common/board_temp_request.hpp"
#include "codes/messages/common/board_temp_response.hpp"
#include "codes/messages/common/fw_version_request.hpp"
#include "codes/messages/common/fw_version_response.hpp"
#include "codes/messages/common/fw_hash_request.hpp"
#include "codes/messages/common/fw_hash_response.hpp"
#include "codes/messages/common/fw_dirty_request.hpp"
#include "codes/messages/common/fw_dirty_response.hpp"
#include "codes/messages/common/hw_version_request.hpp"
#include "codes/messages/common/hw_version_response.hpp"

#include <Poco/Clock.h>
#include <iostream>

CanCommonModule::CanCommonModule(ModuleID id, ICanChannel::Ptr channel) : base(id, channel){

}

ModuleID CanCommonModule::id() const {
    return base.id();
}

std::future<float> CanCommonModule::ping() {
    static std::atomic<uint8_t> sequenceNumber{0};

    Poco::Clock now;
    uint8_t currentSequenceNumber = sequenceNumber++;

    App_messages::Common::Ping_request pingRequest(currentSequenceNumber);

    return base.getWithSeq<
        App_messages::Common::Ping_request,
        App_messages::Common::Ping_response,
        float
    >(pingRequest, [now](App_messages::Common::Ping_response response) {
        float elapsed = now.elapsed() / 1000.0;
        return elapsed;
    }, default_timeout_ms, currentSequenceNumber);
}

std::future <float> CanCommonModule::getCoreLoad() {
    return base.get<
        App_messages::Common::Core_load_request,
        App_messages::Common::Core_load_response,
        float
    >([](App_messages::Common::Core_load_response response){
        return response.load;
    }, default_timeout_ms);
}

std::future <float> CanCommonModule::getCoreTemp() {
    return base.get<
        App_messages::Common::Core_temp_request,
        App_messages::Common::Core_temp_response,
        float
    >([](App_messages::Common::Core_temp_response response){
        return response.temperature;
    }, default_timeout_ms);
}

std::future <float> CanCommonModule::getBoardTemp() {
    return base.get<
        App_messages::Common::Board_temp_request,
        App_messages::Common::Board_temp_response,
        float
    >([](App_messages::Common::Board_temp_response response){
        return response.temperature;
    }, default_timeout_ms);
}

std::future <bool> CanCommonModule::restartModule() {
    App_messages::Common::Device_reset r;
    return base.set(r);
}

std::future <bool> CanCommonModule::rebootModuleUsbBootloader() {
    App_messages::Common::Device_usb_bootloader r;
    return base.set(r);
}

std::future <bool> CanCommonModule::rebootModuleCanBootloader() {
    App_messages::Common::Device_can_bootloader r;
    return base.set(r);
}

std::future<ICommonModule::FwVersion> CanCommonModule::getFwVersion()
{
    auto f_ver = base.get<
        App_messages::Common::FW_version_request,
        App_messages::Common::FW_version_response,
        std::string
    >(
        App_messages::Common::FW_version_request{},
        [](const App_messages::Common::FW_version_response &res) -> std::string {
            return std::to_string(res.major) + "." +
                   std::to_string(res.minor) + "." +
                   std::to_string(res.patch);
        },
        default_timeout_ms
    );

    auto f_hash = base.get<
        App_messages::Common::FW_hash_request,
        App_messages::Common::FW_hash_response,
        std::string
    >(
        App_messages::Common::FW_hash_request{},
        [](const App_messages::Common::FW_hash_response &res) -> std::string {
            return std::to_string(res.hash);;
        },
        default_timeout_ms
    );

    auto f_dirty = base.get<
        App_messages::Common::FW_dirty_request,
        App_messages::Common::FW_dirty_response,
        bool
    >(
        App_messages::Common::FW_dirty_request{},
        [](const App_messages::Common::FW_dirty_response &res) -> bool {
            return res.dirty;
        },
        default_timeout_ms
    );

    return std::async(std::launch::async,
        [f_ver = std::move(f_ver),
        f_hash = std::move(f_hash),
        f_dirty = std::move(f_dirty)
        ]() mutable -> ICommonModule::FwVersion
        {
            ICommonModule::FwVersion result;
            result.version = f_ver.get();
            result.hash = f_hash.get();
            result.dirty = f_dirty.get();
            return result;
        }
    );
}

std::future<std::string> CanCommonModule::getHwVersion()
{
    return base.get<
        App_messages::Common::HW_version_request,
        App_messages::Common::HW_version_response,
        std::string
    >(
        App_messages::Common::HW_version_request{},
        [](const App_messages::Common::HW_version_response &res) -> std::string {
            return std::to_string(res.major) + "." + std::to_string(res.minor);
        },
        default_timeout_ms
    );
}
