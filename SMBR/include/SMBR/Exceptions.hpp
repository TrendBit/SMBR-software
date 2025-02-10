#pragma once    

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <future>


class Exception : public std::runtime_error {
public:
    Exception(const std::string& message, int code) ;
};

class TimeoutException : public Exception {
public:
    static const int Code = 504;
    TimeoutException(const std::string& message);
};

class NotFoundException : public Exception {
public:
    static const int Code = 404;
    NotFoundException(const std::string& message);
};

class ArgumentException : public Exception {
public:
    static const int Code = 400;
    ArgumentException(const std::string& message);
};




