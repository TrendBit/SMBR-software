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
            response.max_flowrate,
            response.min_flowrate
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
