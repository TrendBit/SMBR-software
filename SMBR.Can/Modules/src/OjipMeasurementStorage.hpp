#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Object.h>
#include <Poco/File.h>
#include <Poco/Path.h>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>

class OjipMeasurementStorage {
    public:
        struct MeasurementParams {
            uint32_t api_id;
            uint16_t samples;
            uint16_t length_ms;
            int timebase;
            bool isRead;
        };

        static uint8_t CalculateMeasurementID(uint32_t api_id);
        static std::pair<bool, std::string> readMeasurementParams(const std::string& filePath, MeasurementParams& params);
        static std::pair<bool, std::string> writeMeasurementParams(const std::string& filePath, const MeasurementParams& params);
        static std::pair<bool, std::string> ensureDirectoryExists(const std::string& filePath);
        static std::string toIso8601(const std::chrono::system_clock::time_point& tp);


};