#include "CanCoreModule.hpp"
#include "codes/codes.hpp"
#include <iostream>       // std::cout
#include <future>         // std::async, std::future

#include <iomanip>
#include <sstream>

#include "codes/messages/core/sid_request.hpp"
#include "codes/messages/core/sid_response.hpp"
#include "codes/messages/core/ip_request.hpp"
#include "codes/messages/core/ip_response.hpp"
#include "codes/messages/core/hostname_request.hpp"
#include "codes/messages/core/hostname_response.hpp"
#include "codes/messages/core/serial_request.hpp"
#include "codes/messages/core/serial_response.hpp"
#include "codes/messages/core/supply_type_request.hpp"
#include "codes/messages/core/supply_type_response.hpp"
#include "codes/messages/core/supply_5v_rail_request.hpp"
#include "codes/messages/core/supply_5v_rail_response.hpp"
#include "codes/messages/core/supply_vin_rail_request.hpp"
#include "codes/messages/core/supply_vin_rail_response.hpp"
#include "codes/messages/core/supply_poe_rail_request.hpp"
#include "codes/messages/core/supply_poe_rail_response.hpp"
#include "codes/messages/core/supply_current_request.hpp"
#include "codes/messages/core/supply_current_response.hpp"
#include "codes/messages/core/supply_power_draw_request.hpp"
#include "codes/messages/core/supply_power_draw_response.hpp"

CanCoreModule::CanCoreModule(std::string uidHex, ICanChannel::Ptr channel) : base({Modules::Core, uidHex}, channel){

}

ModuleID CanCoreModule::id() const {
    return base.id();
}

std::future <std::string> CanCoreModule::getShortID() {
   
    return base.get<
        App_messages::Core::SID_request, 
        App_messages::Core::SID_response, 
        std::string>([](App_messages::Core::SID_response response){
            std::stringstream sidStream;
            sidStream << std::hex << std::setw(4) << std::setfill('0') << response.sid;  
            std::string res = sidStream.str();
            return res;
        }, 1000);
}

std::future <std::string> CanCoreModule::getIpAddress() {
    return base.get<
        App_messages::Core::IP_address_request,
        App_messages::Core::IP_address_response, 
        std::string>([](App_messages::Core::IP_address_response response){
            std::stringstream ipStream;
            ipStream << (int)response.IP_address[0] << "." << (int)response.IP_address[1] << "." << (int)response.IP_address[2] << "." << (int)response.IP_address[3];
            std::string res = ipStream.str();
            return res;
        }, 1000);
}

std::future <std::string> CanCoreModule::getHostname() {
    return base.get<
        App_messages::Core::Hostname_request, 
        App_messages::Core::Hostname_response, 
        std::string>([](App_messages::Core::Hostname_response response){
            std::string res = std::string(response.hostname);
            return res;
        }, 1000);
}

std::future <int64_t> CanCoreModule::getSerialNumber() {
    return base.get<
        App_messages::Core::Serial_request, 
        App_messages::Core::Serial_response, 
        int64_t>([](App_messages::Core::Serial_response response){
            int64_t res = response.serial_number;
            return res;
        }, 1000);
}

std::future <ICoreModule::PowerSupplyType> CanCoreModule::getPowerSupplyType() {
    return base.get<
        App_messages::Core::Supply_type_request, 
        App_messages::Core::Supply_type_response, 
        PowerSupplyType>([](App_messages::Core::Supply_type_response response){
            ICoreModule::PowerSupplyType res;
            res.isVIN = response.vin;
            res.isPoE = response.poe;
            res.isPoE_Hb = response.poe_hb;
            return res;
        }, 1000);
}

std::future <float> CanCoreModule::getVoltage5V() {
    App_messages::Core::Supply_5V_rail_request request;
    App_messages::Core::Supply_5V_rail_response response(0); 
    return base.get<
        App_messages::Core::Supply_5V_rail_request, 
        App_messages::Core::Supply_5V_rail_response, 
        float>(request, response, [](App_messages::Core::Supply_5V_rail_response response){
            float res = response.rail_5v;
            return res;
        }, 1000);
}

std::future <float> CanCoreModule::getVoltageVIN() {
    App_messages::Core::Supply_VIN_rail_request request;
    App_messages::Core::Supply_VIN_rail_response response(0); 

    return base.get<
        App_messages::Core::Supply_VIN_rail_request, 
        App_messages::Core::Supply_VIN_rail_response, 
        float>(request, response, [](App_messages::Core::Supply_VIN_rail_response response){
            float res = response.rail_vin;
            return res;
        }, 1000);
}

std::future <float> CanCoreModule::getVoltagePoE() {
    App_messages::Core::Supply_POE_rail_request request;
    App_messages::Core::Supply_POE_rail_response response(0); 
    return base.get<
        App_messages::Core::Supply_POE_rail_request,
        App_messages::Core::Supply_POE_rail_response, 
        float>(request, response, [](App_messages::Core::Supply_POE_rail_response response){
            float res = response.rail_poe;
            return res;
        }, 1000);
}

std::future <float> CanCoreModule::getCurrentConsumption() {
    App_messages::Core::Supply_current_request request;
    App_messages::Core::Supply_current_response response(0); 
    return base.get<
        App_messages::Core::Supply_current_request, 
        App_messages::Core::Supply_current_response, 
        float>(request, response, [](App_messages::Core::Supply_current_response response){
            float res = response.current;
            return res;
        }, 1000);
}

std::future <float> CanCoreModule::getPowerDraw() {
    App_messages::Core::Supply_power_draw_request request;
    App_messages::Core::Supply_power_draw_response response(0); 
    return base.get<
        App_messages::Core::Supply_power_draw_request, 
        App_messages::Core::Supply_power_draw_response, 
        float>(request, response, [](App_messages::Core::Supply_power_draw_response response){
            float res = response.power_draw;
            return res;
        }, 1000);
}




