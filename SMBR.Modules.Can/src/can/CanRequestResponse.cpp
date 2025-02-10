#include "CanRequest.hpp"

CanRequest::CanRequest(RequestData requestData) : requestData_(requestData){

}

CanRequest::CanRequest(RequestData requestData, ResponseInfo responseInfo) : requestData_(requestData), responseInfo_(responseInfo){

}

