#include "TMP1075.hpp"

TMP1075::TMP1075(I2C_bus &bus, unsigned char address) :
    I2C_device(bus, address)
{ }

double TMP1075::Temperature(){
    Write({0x00});
    auto temp_raw = Read(2);
    if(not temp_raw.has_value()){
        return 0;
    }
    return ((temp_raw.value()[0] << 4) + (temp_raw.value()[1] >> 4)) * 0.0625;
}
