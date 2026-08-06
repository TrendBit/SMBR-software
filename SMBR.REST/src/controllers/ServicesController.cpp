#include "ServicesController.hpp"

#include "SMBR/Exceptions.hpp"
#include "SMBR/Log.hpp"
#include "ControllerUtils.hpp"

#include <Poco/Process.h>
#include <Poco/PipeStream.h>
#include <Poco/StreamCopier.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Exception.h>
#include <sstream>
#include <fstream>
#include <cstdio>

#include <chrono>

using namespace std::chrono_literals;

namespace {

const char* const SWUPDATE_WEB_HOST = "127.0.0.1";
const Poco::UInt16 SWUPDATE_WEB_PORT = 8080;
const char* const SWUPDATE_TEMP_UPLOAD_PATH = "/tmp/smbr-swupdate-upload.swu";

/**
 * @brief Deletes the given file when it goes out of scope, regardless of how the scope is exited.
 */
struct TempFileGuard {
    std::string path;
    ~TempFileGuard() { std::remove(path.c_str()); }
};

/**
 * @brief Writes the raw HTTP request body to a local file, chunk by chunk.
 */
class FileWriteCallback : public oatpp::data::stream::WriteCallback {
public:
    explicit FileWriteCallback(std::ofstream& file) : m_file(file) {}

    oatpp::v_io_size write(const void* data, v_buff_size count, oatpp::async::Action&) override {
        if (count <= 0) {
            return 0;
        }
        m_file.write(static_cast<const char*>(data), count);
        if (!m_file) {
            return oatpp::IOError::BROKEN_PIPE;
        }
        m_totalBytes += count;
        return count;
    }

    oatpp::v_io_size totalBytes() const { return m_totalBytes; }

private:
    std::ofstream& m_file;
    oatpp::v_io_size m_totalBytes = 0;
};

}

ServicesController::ServicesController(const std::shared_ptr<oatpp::web::mime::ContentMappers>& apiContentMappers,
                                       std::shared_ptr<ISystemModule> systemModule)
    : SMBRControllerBase(apiContentMappers, systemModule)
{}

std::string ServicesController::serviceUnitName(const oatpp::Enum<dto::ServiceEnum>::AsString& service) {
    if (service == dto::ServiceEnum::core_module) return "reactor-core-module.service";
    if (service == dto::ServiceEnum::api_server) return "reactor-api-server.service";
    if (service == dto::ServiceEnum::web_control_ts) return "reactor-web-control-ts.service";
    if (service == dto::ServiceEnum::database_export) return "reactor-database-export.service";
    if (service == dto::ServiceEnum::startup_updates) return "reactor-startup-updates.service";
    if (service == dto::ServiceEnum::can0) return "can0.service";
    if (service == dto::ServiceEnum::avahi_daemon) return "avahi-daemon.service";
    if (service == dto::ServiceEnum::swupdate) return "swupdate.service";
    if (service == dto::ServiceEnum::telegraf) return "telegraf.service";
    throw ArgumentException("Unknown service");
}

static const std::vector<std::string>& managedServiceUnitNames() {
    static const std::vector<std::string> names = {
        "reactor-core-module.service",
        "reactor-api-server.service",
        "reactor-web-control-ts.service",
        "reactor-database-export.service",
        "reactor-startup-updates.service",
        "can0.service",
        "avahi-daemon.service",
        "swupdate.service",
        "telegraf.service"
    };
    return names;
}

ServicesController::SystemdUnitStatus ServicesController::querySystemdUnit(const std::string& unitName) {
    Poco::Pipe outPipe;
    Poco::Process::Args args{
        "show", unitName, "--no-pager",
        "--property=LoadState,ActiveState,SubState,UnitFileState,MainPID,ExecMainPID,"
        "ActiveEnterTimestamp,ActiveExitTimestamp,InactiveEnterTimestamp,InactiveExitTimestamp"
    };

    Poco::ProcessHandle ph = Poco::Process::launch("systemctl", args, nullptr, &outPipe, nullptr);

    Poco::PipeInputStream istr(outPipe);
    std::stringstream output;
    Poco::StreamCopier::copyStream(istr, output);

    int exitCode = ph.wait();
    if (exitCode != 0) {
        throw std::runtime_error("systemctl exited with code " + std::to_string(exitCode));
    }

    SystemdUnitStatus status;
    int32_t execMainPid = 0;
    std::string activeEnter, activeExit, inactiveEnter, inactiveExit;
    std::string line;
    while (std::getline(output, line)) {
        auto pos = line.find('=');
        if (pos == std::string::npos) continue;
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        if (key == "LoadState") status.loadState = value;
        else if (key == "ActiveState") status.activeState = value;
        else if (key == "SubState") status.subState = value;
        else if (key == "UnitFileState") status.unitFileState = value;
        else if (key == "ActiveEnterTimestamp") activeEnter = value;
        else if (key == "ActiveExitTimestamp") activeExit = value;
        else if (key == "InactiveEnterTimestamp") inactiveEnter = value;
        else if (key == "InactiveExitTimestamp") inactiveExit = value;
        else if (key == "MainPID") {
            try { status.mainPid = std::stoi(value); }
            catch (...) { status.mainPid = 0; }
        } else if (key == "ExecMainPID") {
            try { execMainPid = std::stoi(value); }
            catch (...) { execMainPid = 0; }
        }
    }

    if (status.mainPid == 0) {
        status.mainPid = execMainPid;
    }

    if (status.activeState == "active" || status.activeState == "reloading") {
        status.since = activeEnter;
    } else if (status.activeState == "activating") {
        status.since = inactiveExit;
    } else if (status.activeState == "deactivating") {
        status.since = activeExit;
    } else {
        status.since = inactiveEnter;
    }
    return status;
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ServicesController::getServiceStatus(const oatpp::Enum<dto::ServiceEnum>::AsString& service) {
    return process(__FUNCTION__, [&]() {
        std::string unitName = serviceUnitName(service);

        SystemdUnitStatus status;
        try {
            status = querySystemdUnit(unitName);
        } catch (std::exception& e) {
            throw std::runtime_error("Failed to retrieve status: " + std::string(e.what()));
        }

        if (status.loadState == "not-found") {
            throw NotFoundException("Unit '" + unitName + "' not found");
        }

        return createDtoResponse(Status::CODE_200, toServiceStatusDto(unitName, status));
    });
}

oatpp::Object<ServiceStatusDto> ServicesController::toServiceStatusDto(const std::string& unitName, const SystemdUnitStatus& status) {
    auto dto = ServiceStatusDto::createShared();
    dto->name = unitName;
    dto->load_state = status.loadState;
    dto->active_state = status.activeState;
    dto->sub_state = status.subState;
    dto->enabled = (status.unitFileState == "enabled" || status.unitFileState == "enabled-runtime");
    dto->main_pid = status.mainPid;
    dto->since = status.since;
    return dto;
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ServicesController::getServiceStatuses() {
    return process(__FUNCTION__, [&]() {
        auto list = oatpp::List<oatpp::Object<ServiceStatusDto>>::createShared();
        for (const auto& unitName : managedServiceUnitNames()) {
            SystemdUnitStatus status;
            try {
                status = querySystemdUnit(unitName);
            } catch (std::exception& e) {
                LWARNING("API") << "Api getServiceStatuses failed to query unit " << unitName << ": " << e.what() << LE;
                status = SystemdUnitStatus();
                status.loadState = "not-found";
            }
            list->push_back(toServiceStatusDto(unitName, status));
        }
        return createDtoResponse(Status::CODE_200, list);
    });
}

std::vector<std::string> ServicesController::queryServiceLogs(const std::string& unitName, int lineCount) {
    Poco::Pipe outPipe;
    Poco::Process::Args args{
        "-u", unitName, "-n", std::to_string(lineCount), "--no-pager"
    };

    Poco::ProcessHandle ph = Poco::Process::launch("journalctl", args, nullptr, &outPipe, nullptr);

    Poco::PipeInputStream istr(outPipe);
    std::stringstream output;
    Poco::StreamCopier::copyStream(istr, output);

    int exitCode = ph.wait();
    if (exitCode != 0) {
        throw std::runtime_error("journalctl exited with code " + std::to_string(exitCode));
    }

    std::vector<std::string> result;
    std::string line;
    while (std::getline(output, line)) {
        if (!line.empty()) {
            result.push_back(line);
        }
    }
    return result;
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ServicesController::getServiceLogs(const std::shared_ptr<IncomingRequest>& request, const oatpp::Enum<dto::ServiceEnum>::AsString& service) {
    return process(__FUNCTION__, [&]() {
        std::string unitName = serviceUnitName(service);

        int lineCount = 100;
        auto linesParam = request->getQueryParameter("lines");
        if (linesParam) {
            try { lineCount = std::stoi(linesParam->c_str()); }
            catch (...) { throw ArgumentException("lines must be an integer between 1 and 1000"); }
            if (lineCount < 1 || lineCount > 1000) {
                throw ArgumentException("lines must be between 1 and 1000");
            }
        }

        SystemdUnitStatus status;
        try {
            status = querySystemdUnit(unitName);
        } catch (std::exception& e) {
            throw std::runtime_error("Failed to retrieve status: " + std::string(e.what()));
        }
        if (status.loadState == "not-found") {
            throw NotFoundException("Unit '" + unitName + "' not found");
        }

        std::vector<std::string> logLines;
        try {
            logLines = queryServiceLogs(unitName, lineCount);
        } catch (std::exception& e) {
            throw std::runtime_error("Failed to retrieve logs: " + std::string(e.what()));
        }

        auto dto = ServiceLogsDto::createShared();
        dto->name = unitName;
        dto->lines = oatpp::List<oatpp::String>::createShared();
        for (const auto& l : logLines) {
            dto->lines->push_back(l);
        }
        return createDtoResponse(Status::CODE_200, dto);
    });
}

void ServicesController::runSystemctlAction(const std::string& unitName, const std::string& action) {
    Poco::Pipe errPipe;
    Poco::Process::Args args{action, unitName};

    Poco::ProcessHandle ph = Poco::Process::launch("systemctl", args, nullptr, nullptr, &errPipe);

    Poco::PipeInputStream istr(errPipe);
    std::stringstream errOutput;
    Poco::StreamCopier::copyStream(istr, errOutput);

    int exitCode = ph.wait();
    if (exitCode != 0) {
        std::string err = errOutput.str();
        while (!err.empty() && (err.back() == '\n' || err.back() == '\r')) err.pop_back();
        std::string message = "systemctl exited with code " + std::to_string(exitCode);
        if (!err.empty()) message += ": " + err;
        throw std::runtime_error(message);
    }
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ServicesController::performServiceAction(
    const oatpp::Enum<dto::ServiceEnum>::AsString& service,
    const std::string& systemctlAction,
    const std::string& pastTenseVerb)
{
    std::string unitName = serviceUnitName(service);

    SystemdUnitStatus status;
    try {
        status = querySystemdUnit(unitName);
    } catch (std::exception& e) {
        throw std::runtime_error("Failed to retrieve status: " + std::string(e.what()));
    }
    if (status.loadState == "not-found") {
        throw NotFoundException("Unit '" + unitName + "' not found");
    }

    try {
        runSystemctlAction(unitName, systemctlAction);
    } catch (std::exception& e) {
        throw std::runtime_error("Failed to " + systemctlAction + " service: " + std::string(e.what()));
    }

    auto dto = MessageDto::createShared();
    dto->message = "Successfully " + pastTenseVerb + " " + unitName;
    return createDtoResponse(Status::CODE_200, dto);
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ServicesController::startService(const oatpp::Enum<dto::ServiceEnum>::AsString& service) {
    return process(__FUNCTION__, [&]() {
        return performServiceAction(service, "start", "started");
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ServicesController::stopService(const oatpp::Enum<dto::ServiceEnum>::AsString& service) {
    return process(__FUNCTION__, [&]() {
        return performServiceAction(service, "stop", "stopped");
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ServicesController::restartService(const oatpp::Enum<dto::ServiceEnum>::AsString& service) {
    return process(__FUNCTION__, [&]() {
        return performServiceAction(service, "restart", "restarted");
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ServicesController::enableService(const oatpp::Enum<dto::ServiceEnum>::AsString& service) {
    return process(__FUNCTION__, [&]() {
        return performServiceAction(service, "enable", "enabled");
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ServicesController::disableService(const oatpp::Enum<dto::ServiceEnum>::AsString& service) {
    return process(__FUNCTION__, [&]() {
        return performServiceAction(service, "disable", "disabled");
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ServicesController::triggerSwUpdate(const std::shared_ptr<IncomingRequest>& request) {
    return process(__FUNCTION__, [&]() -> std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> {
        TempFileGuard tempFile{SWUPDATE_TEMP_UPLOAD_PATH};
        oatpp::v_io_size totalBytes = 0;
        {
            std::ofstream file(tempFile.path, std::ios::binary | std::ios::trunc);
            if (!file) {
                throw std::runtime_error("Failed to open a temporary file to buffer the upload");
            }
            auto sink = std::make_shared<FileWriteCallback>(file);
            request->transferBody(sink);
            totalBytes = sink->totalBytes();
        }

        if (totalBytes == 0) {
            throw ArgumentException("Request body is empty; expected the .swu update image");
        }

        static const std::string boundary = "----SMBRSwUpdateBoundary7f3a9c2e";
        const std::string preamble =
            "--" + boundary + "\r\n"
            "Content-Disposition: form-data; name=\"file\"; filename=\"update.swu\"\r\n"
            "Content-Type: application/octet-stream\r\n\r\n";
        const std::string epilogue = "\r\n--" + boundary + "--\r\n";

        std::ifstream uploadFile(tempFile.path, std::ios::binary);
        if (!uploadFile) {
            throw std::runtime_error("Failed to reopen the buffered upload to forward it to SWUpdate");
        }

        Poco::Net::HTTPClientSession session(SWUPDATE_WEB_HOST, SWUPDATE_WEB_PORT);
        session.setTimeout(Poco::Timespan(300, 0));

        Poco::Net::HTTPRequest httpRequest(Poco::Net::HTTPRequest::HTTP_POST, "/upload", Poco::Net::HTTPMessage::HTTP_1_1);
        httpRequest.setContentType("multipart/form-data; boundary=" + boundary);
        httpRequest.setContentLength(static_cast<std::streamsize>(preamble.size() + totalBytes + epilogue.size()));

        std::ostream* os;
        try {
            os = &session.sendRequest(httpRequest);
        } catch (Poco::Exception& e) {
            throw std::runtime_error("Failed to connect to SWUpdate's web UI on 127.0.0.1:8080: " + e.displayText());
        }

        std::stringstream responseBody;
        Poco::Net::HTTPResponse::HTTPStatus status;
        try {
            *os << preamble;
            Poco::StreamCopier::copyStream(uploadFile, *os);
            *os << epilogue;

            Poco::Net::HTTPResponse httpResponse;
            std::istream& rs = session.receiveResponse(httpResponse);
            Poco::StreamCopier::copyStream(rs, responseBody);
            status = httpResponse.getStatus();
        } catch (Poco::Exception& e) {
            throw ArgumentException("SWUpdate rejected the uploaded file - it is likely not a valid .swu image (" + e.displayText() + ")");
        }

        if (status != Poco::Net::HTTPResponse::HTTP_OK) {
            throw ArgumentException("SWUpdate rejected the update image (HTTP " + std::to_string(static_cast<int>(status)) + "): " + responseBody.str());
        }

        auto dto = MessageDto::createShared();
        dto->message = "Update image uploaded. Installation in progress, check /services/swupdate/logs";
        return createDtoResponse(Status::CODE_202, dto);
    });
}
