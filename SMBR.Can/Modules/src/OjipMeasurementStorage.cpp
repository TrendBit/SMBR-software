#include "OjipMeasurementStorage.hpp"

uint8_t OjipMeasurementStorage::CalculateMeasurementID(uint32_t api_id) {
    return (api_id % 15) + 1;
}

std::pair<bool, std::string> OjipMeasurementStorage::ensureDirectoryExists(const std::string& filePath) {
    try {
        Poco::Path path(filePath);
        if (path.isDirectory()) {
            return {true, ""};
        }

        Poco::File dir(path.parent());
        if (!dir.exists()) {
            dir.createDirectories();
        }
        return {dir.exists(), ""};
    } catch (const Poco::Exception& e) {
        std::string error = "Directory error [" + filePath + "]: " + e.displayText();
        return {false, error};
    }
}

std::pair<bool, std::string> OjipMeasurementStorage::readMeasurementParams(const std::string& filePath, MeasurementParams& params) {
    auto [dirSuccess, dirError] = ensureDirectoryExists(filePath);
    if (!dirSuccess) {
        return {false, dirError};
    }

    try {
        Poco::File file(filePath);
        if (!file.exists()) {
            return {false, "File does not exist: " + filePath};
        }
        if (file.getSize() == 0) {
            return {false, "File is empty: " + filePath};
        }

        std::ifstream input(filePath);
        if (!input.is_open()) {
            return {false, "Failed to open file: " + filePath};
        }

        Poco::JSON::Parser parser;
        auto result = parser.parse(input);
        input.close();

        auto object = result.extract<Poco::JSON::Object::Ptr>();
        if (!object) {
            return {false, "Invalid JSON structure in file: " + filePath};
        }

        static const std::vector<std::string> requiredFields = {
            "api_id", "samples", "length_ms", "timebase", "isRead"
        };

        for (const auto& field : requiredFields) {
            if (!object->has(field)) {
                return {false, "Missing field '" + field + "' in JSON file: " + filePath};
            }
        }

        try {
            params.api_id = object->getValue<int>("api_id");
            params.samples = object->getValue<int>("samples");
            params.length_ms = object->getValue<int>("length_ms");
            params.timebase = object->getValue<int>("timebase");
            params.isRead = object->getValue<bool>("isRead");
        } catch (const Poco::Exception& e) {
            return {false, "Type mismatch in JSON file [" + filePath + "]: " + e.displayText()};
        }

        return {true, ""};
    } catch (const Poco::Exception& e) {
        return {false, "JSON parse error [" + filePath + "]: " + e.displayText()};
    } catch (const std::exception& e) {
        return {false, "System error [" + filePath + "]: " + e.what()};
    } catch (...) {
        return {false, "Unknown error processing file: " + filePath};
    }
}

std::pair<bool, std::string> OjipMeasurementStorage::writeMeasurementParams(const std::string& filePath, const MeasurementParams& params) {
    auto [dirSuccess, dirError] = ensureDirectoryExists(filePath);
    if (!dirSuccess) {
        return {false, dirError};
    }

    const std::string tempPath = filePath + ".tmp";
    try {
        {
            std::ofstream output(tempPath, std::ios::binary);
            if (!output.is_open()) {
                return {false, "Failed to create temp file: " + tempPath};
            }

            Poco::JSON::Object json;
            json.set("api_id", params.api_id);
            json.set("samples", params.samples);
            json.set("length_ms", params.length_ms);
            json.set("timebase", params.timebase);
            json.set("isRead", params.isRead);

            Poco::JSON::Stringifier::stringify(json, output);
            output.close();

            if (output.fail()) {
                Poco::File(tempPath).remove();
                return {false, "Write error for temp file: " + tempPath};
            }
        }

        Poco::File tempFile(tempPath);
        tempFile.renameTo(filePath);

        return {true, ""};
    } catch (const Poco::Exception& e) {
        Poco::File(tempPath).remove();
        return {false, "Write operation failed [" + filePath + "]: " + e.displayText()};
    } catch (const std::exception& e) {
        Poco::File(tempPath).remove();
        return {false, "System error [" + filePath + "]: " + e.what()};
    } catch (...) {
        Poco::File(tempPath).remove();
        return {false, "Unknown error writing file: " + filePath};
    }
}


std::string OjipMeasurementStorage::toIso8601(const std::chrono::system_clock::time_point& tp) {
    std::time_t timeT = std::chrono::system_clock::to_time_t(tp);
    long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        tp.time_since_epoch()).count() % 1000;

    std::ostringstream oss;
    oss << std::put_time(std::localtime(&timeT), "%Y-%m-%dT%H:%M:%S");
    return oss.str();
}
