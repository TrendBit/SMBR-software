#include "SMBR/ICoreModule.hpp"

std::ostream& operator<<(std::ostream& os, const ICoreModule::PowerSupplyType & v){
    os << "PowerSupplyType: {";
    os << " isVIN: " << v.isVIN;
    os << " isPoE: " << v.isPoE;
    os << " isPoE_Hb: " << v.isPoE_Hb;
    os << "}";

    return os;
}

