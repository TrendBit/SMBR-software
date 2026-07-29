#pragma once

#include "oatpp/Types.hpp"

#include <chrono>
#include <future>
#include <string>

/**
 * @brief Waits for the future and returns its value.
 *
 * Timeout is removed, since it is not needed - CAN messages itself should have
 * better timeout handling.
 */
template <class T>
T waitFor(std::future<T> future){
    future.wait();
    return future.get();
}

/**
 * @brief Decodes the URI encoded recipe name taken from the request path.
 */
std::string decodeRecipeName(const oatpp::String& name);
