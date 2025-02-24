#include "ICanChannel.hpp"
#include <Poco/NumberFormatter.h>

std::ostream & operator<<(std::ostream & os, const ICanChannel::Response & r){
    os << "[";
    for (auto d : r.responseData) {
        os << d.id << ": [";
        for (auto rd : d.data){
            os << Poco::NumberFormatter::formatHex(rd, true) << " ";
        }
        os << "] ";
    }
    os << "]";
    return os;
}
