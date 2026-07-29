#include "ControllerUtils.hpp"

#include <Poco/URI.h>

std::string decodeRecipeName(const oatpp::String& name){
    std::string inStr = name;
    std::string outStr;
    Poco::URI::decode(inStr, outStr);
    return outStr;
}
