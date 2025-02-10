#pragma once

#include <memory>
#include <vector>
#include <optional>
#include <functional>
#include <iostream>

/**
 * @enum CanRequestStatus
 * @brief Enum representing the status of a CAN request.
 */
enum class CanRequestStatus { Unknown, Success, Timeout, Fail };

typedef std::vector<uint8_t> Data;

typedef uint32_t MessageID;

struct CanID {
    MessageID id = 0x00;
    CanID(MessageID id) : id(id) {}


    enum ModuleAddress {
        Undefined = 0x00,
        All = 0x01,
        Any = 0x02,
        Test_module = 0x03,
        Core_module = 0x04,
        Control_module = 0x05,
        Sensor_module = 0x06,        
    };

    bool forAll() const {
        return moduleAddress() == All;
    }

    ModuleAddress moduleAddress() const {
        return static_cast<ModuleAddress>(address());
    }

    uint8_t address() const {
        return (uint8_t)((id >> 4) & 0xFF);
    }

    uint8_t instance() const {
        return id & 0xF;
    }

    bool isEmergency() const {
        return (id >> 28) & 1;
    }

    u_int16_t messageType() const {
        return (id >> 16) & 0xFFF;
    }

    std::string toString() const;
    std::string toHexString() const;
    std::string toReadableString() const;

    bool operator==(const CanID & o) const {
        return id == o.id;
    }
    bool operator!=(const CanID & o) const {
        return id != o.id;
    }
    bool operator<(const CanID & o) const {
        return id < o.id;
    }
};

struct RequestData {
    CanID id;
    Data data;

    RequestData(CanID id, Data data) : id(id), data(data) {}
};


struct ResponseData {
    CanID id;
    Data data;

    ResponseData();
    ResponseData(CanID id, Data data) : id(id), data(data) {}
    
};

struct ResponseInfo {
    int timeoutMs = 1000;
    
    std::function <bool(const ResponseData &)> acceptFunction;
    std::function <bool(const std::vector <ResponseData> &)> isDoneFunction;

    bool successOnTimeout = false;

    ResponseInfo() {
        acceptFunction = [](const ResponseData & response) {
            return false;
        };
        isDoneFunction = [](const std::vector <ResponseData> & responses) {
            return true;
        };
    }

    ResponseInfo(CanID responseId);
    ResponseInfo(CanID responseId, int timeoutMs);
    
};




/**
 * @class CanRequest
 * @brief Class representing a CAN bus request.
 * 
 * CanRequest handles the sending of CAN bus requests and the receiving of responses,
 * including support for multiple responses.
 */
class CanRequest {
public:
    
    
    CanRequest(RequestData requestData);
    CanRequest(RequestData requestData, ResponseInfo responseInfo);

    const RequestData & request() const ;
    const ResponseInfo & responseInfo() const;    

private:
    RequestData requestData_;
    ResponseInfo responseInfo_;
};

std::ostream& operator<<(std::ostream& os, const CanID& canid);
std::ostream& operator<<(std::ostream& os, const CanRequestStatus& requestData);

