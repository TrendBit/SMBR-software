#include "CanRequest.hpp"
#include <sstream>
#include <iomanip>


std::string CanID::toString() const {
    return toHexString() + " (" + toReadableString() + ")";
}

std::string CanID::toHexString() const {
    std::stringstream ss;
    ss << "0x" << std::hex << std::setw(8) << std::setfill('0') << id;
    return ss.str();
}
std::string CanID::toReadableString() const {
    std::stringstream ss;
    ss << "0x" << std::setw(4) << std::setfill('0') << messageType() << " ";
    auto a = address();
    if (a == 0x00) {
        ss << "UND";
    } else if (a == 0x01) {
        ss << "ALL";
    } else if (a == 0x02) {
        ss << "ANY";
    } else if (a == 0x03) {
        ss << "TES";
    } else if (a == 0x04) {
        ss << "COR";
    } else if (a == 0x05) {
        ss << "CON";
    } else if (a == 0x06) {
        ss << "SEN";
    } else {
        ss << "???";
    }
    return ss.str();
}

ResponseInfo::ResponseInfo(CanID responseId){
        acceptFunction = [responseId](const ResponseData & response) {
            std::cout << "a try accept " <<  responseId << " vs " << response.id << std::endl;
            return response.id == responseId;
        };
        isDoneFunction = [](const std::vector <ResponseData> & responses) {
            std::cout << "a is done" << std::endl;
            return responses.size() > 0;
        };
    }
    
    ResponseInfo::ResponseInfo(CanID responseId, int timeoutMs) : timeoutMs(timeoutMs) {
        acceptFunction = [responseId](const ResponseData & response) {
            std::cout << "b try accept " <<  responseId << " vs " << response.id << std::endl;
            return response.id == responseId;
        };
        isDoneFunction = [](const std::vector <ResponseData> & responses) {
            std::cout << "b is done" << std::endl;
            return responses.size() > 0;
        };
    }

CanRequest::CanRequest(RequestData requestData) : requestData_(requestData){

}

CanRequest::CanRequest(RequestData requestData, ResponseInfo responseInfo) : requestData_(requestData), responseInfo_(responseInfo){

}

const RequestData & CanRequest::request() const {
    return requestData_;
}

const ResponseInfo & CanRequest::responseInfo() const {
    return responseInfo_;
}


std::ostream& operator<<(std::ostream& os, const CanID& canid){
    os << canid.toString();
    return os;
}

std::ostream& operator<<(std::ostream& os, const CanRequestStatus& s){
    switch (s) {
        case CanRequestStatus::Success:
            os << "Success";
            break;
        case CanRequestStatus::Fail:
            os << "Fail";
            break;
        case CanRequestStatus::Timeout:
            os << "Timeout";
            break;
        default:
            os << "Unknown";
            break; 
    }
    return os;
}
