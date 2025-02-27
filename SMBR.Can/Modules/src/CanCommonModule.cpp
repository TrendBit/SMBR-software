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

#include <Poco/Clock.h>
#include <iostream>

CanCommonModule::CanCommonModule(ModuleID id, ICanChannel::Ptr channel) : base(id, channel){

}

ModuleID CanCommonModule::id() const {
    return base.id();
}

std::future <float> CanCommonModule::ping() {
    Poco::Clock now;
    return base.get<
        App_messages::Common::Ping_request, 
        App_messages::Common::Ping_response, 
        float
    >([now](App_messages::Common::Ping_response response){
        float elapsed = now.elapsed() / 1000.0;
        return elapsed;
    }, 1000);
}

std::future <float> CanCommonModule::getCoreLoad() {
    return base.get<
        App_messages::Common::Core_load_request, 
        App_messages::Common::Core_load_response, 
        float
    >([](App_messages::Common::Core_load_response response){
        return response.load;
    }, 1000);
}

std::future <float> CanCommonModule::getCoreTemp() {
    return base.get<
        App_messages::Common::Core_temp_request, 
        App_messages::Common::Core_temp_response, 
        float
    >([](App_messages::Common::Core_temp_response response){
        return response.temperature;
    }, 1000);
}

std::future <float> CanCommonModule::getBoardTemp() {
    return base.get<
        App_messages::Common::Board_temp_request, 
        App_messages::Common::Board_temp_response, 
        float
    >([](App_messages::Common::Board_temp_response response){
        return response.temperature;
    }, 1000);
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

