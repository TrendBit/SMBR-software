#include "CanPumpsModule.hpp"
#include "codes/messages/pumps/pump_count_request.hpp"
#include "codes/messages/pumps/pump_count_response.hpp"
#include "codes/messages/pumps/info_request.hpp"
#include "codes/messages/pumps/info_response.hpp"
#include "codes/messages/pumps/get_speed_request.hpp"
#include "codes/messages/pumps/get_speed_response.hpp"
#include "codes/messages/pumps/set_speed.hpp"
#include "codes/messages/pumps/get_flowrate_request.hpp"
#include "codes/messages/pumps/get_flowrate_response.hpp"
#include "codes/messages/pumps/set_flowrate.hpp"
#include "codes/messages/pumps/set_max_flowrate.hpp"
#include "codes/messages/pumps/move.hpp"
#include "codes/messages/pumps/stop.hpp"
#include "codes/messages/enumerator/enumerator_set.hpp"
#include <SMBR/Log.hpp>

using namespace Codes;

CanPumpsModule::CanPumpsModule(ModuleID id, ICanChannel::Ptr channel)
    : base(id, channel), channel(channel) {
}

ModuleID CanPumpsModule::id() const {
    return base.id();
}

std::future<uint8_t> CanPumpsModule::getPumpCount() {
    
    return base.get<
        App_messages::Pumps::Pump_count_request,
        App_messages::Pumps::Pump_count_response,
        uint8_t
    >([](App_messages::Pumps::Pump_count_response response){
        return response.pump_count;
    }, 2000);
}

std::future<IPumpsModule::PumpInfo> CanPumpsModule::getPumpInfo(uint8_t pump_index) {
    App_messages::Pumps::Info_request request(pump_index);
    
    return base.get<
        App_messages::Pumps::Info_request,
        App_messages::Pumps::Info_response,
        IPumpsModule::PumpInfo
    >(request, [](App_messages::Pumps::Info_response response){
        return IPumpsModule::PumpInfo{
            response.min_flowrate,
            response.max_flowrate
        };
    }, 2000);
}

std::future<float> CanPumpsModule::getSpeed(uint8_t pump_index) {
    App_messages::Pumps::Get_speed_request request(pump_index);
    
    return base.get<
        App_messages::Pumps::Get_speed_request,
        App_messages::Pumps::Get_speed_response,
        float
    >(request, [](App_messages::Pumps::Get_speed_response response){
        return response.pump_speed;
    }, 2000);
}

std::future<bool> CanPumpsModule::setSpeed(uint8_t pump_index, float speed) {
    App_messages::Pumps::Set_speed request(pump_index, speed);
    return base.set<App_messages::Pumps::Set_speed>(request);
}

std::future<float> CanPumpsModule::getFlowrate(uint8_t pump_index) {
    App_messages::Pumps::Get_flowrate_request request(pump_index);
    
    return base.get<
        App_messages::Pumps::Get_flowrate_request,
        App_messages::Pumps::Get_flowrate_response,
        float
    >(request, [](App_messages::Pumps::Get_flowrate_response response){
        return response.flowrate;
    }, 2000);
}

std::future<bool> CanPumpsModule::setFlowrate(uint8_t pump_index, float flowrate) {
    App_messages::Pumps::Set_flowrate request(pump_index, flowrate);
    return base.set<App_messages::Pumps::Set_flowrate>(request);
}

std::future<bool> CanPumpsModule::setMaxFlowrate(uint8_t pump_index, float flowrate) {
    App_messages::Pumps::Set_max_flowrate request(pump_index, flowrate);
    return base.set<App_messages::Pumps::Set_max_flowrate>(request);
}

std::future<bool> CanPumpsModule::move(uint8_t pump_index, float volume, float flowrate) {
    App_messages::Pumps::Move request(pump_index, volume, flowrate);
    return base.set<App_messages::Pumps::Move>(request);
}

std::future<bool> CanPumpsModule::stop(uint8_t pump_index) {
    App_messages::Pumps::Stop request(pump_index);
    return base.set<App_messages::Pumps::Stop>(request);
}

std::future<bool> CanPumpsModule::setInstance(uint8_t target_instance) {
    std::string uidHex = base.uidHex();
    
    UID_t uid = {0, 0, 0, 0, 0, 0};
    
    size_t startIdx = 0;
    if (uidHex.length() >= 2 && uidHex[0] == '0' && uidHex[1] == 'x') {
        startIdx = 2;
    }
    
    if (uidHex.length() - startIdx != 12) {
        return std::async(std::launch::deferred, []() {
            return false;
        });
    }
    
    for (int i = 0; i < 6; i++) {
        std::string byte_str = uidHex.substr(startIdx + (i * 2), 2);
        uid[i] = static_cast<uint8_t>(std::strtol(byte_str.c_str(), nullptr, 16));
    }
    
    Codes::Instance target = static_cast<Codes::Instance>(target_instance);
    App_messages::Common::Enumerator_set request(target, uid);
    
    return base.set<App_messages::Common::Enumerator_set>(request);
}
