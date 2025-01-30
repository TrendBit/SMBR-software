#include "SMBR/Exceptions.hpp"

Exception::Exception(const std::string& message, int code) : std::runtime_error(message) {

}

TimeoutException::TimeoutException(const std::string& message) : Exception(message, Code) {

}

NotFoundException::NotFoundException(const std::string& message) : Exception(message, Code) {

}

ArgumentException::ArgumentException(const std::string& message) : Exception(message, Code) {

}


