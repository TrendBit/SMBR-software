#pragma once

#include <memory>
#include <vector>
#include "CanRequest.hpp"

class CanRequestResponse {
public:
    typedef std::vector<uint8_t> Data;


    CanRequestResponse(CanRequest request);
    
    CanRequest request(){
        
    };


private:
    CanRequest request_;
    
};
