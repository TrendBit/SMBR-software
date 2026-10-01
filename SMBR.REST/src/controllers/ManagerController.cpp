#include "ManagerController.hpp"

#include "SMBR/Exceptions.hpp"

namespace {
    constexpr size_t MaxManagerIdLength = 64;

    /** @brief How long a managed=true assertion stays valid without being refreshed by the manager. */
    constexpr std::chrono::seconds ManagedTimeout{90};

    bool isValidManagerId(const std::string & managerId) {
        return !managerId.empty() && managerId.size() <= MaxManagerIdLength;
    }
}

ManagerController::ManagerController(const std::shared_ptr<oatpp::web::mime::ContentMappers>& apiContentMappers,
                                     std::shared_ptr<ISystemModule> systemModule,
                                     std::shared_ptr<ControlledState> controlledState)
    : SMBRControllerBase(apiContentMappers, systemModule)
    , controlledState_(controlledState)
{}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ManagerController::getManaged() {
    return process(__FUNCTION__, [&](){
        Flag flag;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            flag = managed_;
            if (std::chrono::steady_clock::now() - managedSetAt_ > ManagedTimeout) {
                flag = Flag();
            }
        }
        auto dto = ManagedDto::createShared();
        dto->managed = flag.active;
        dto->manager_id = flag.active ? oatpp::String(flag.managerId) : nullptr;
        return createDtoResponse(Status::CODE_200, dto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ManagerController::setManaged(const oatpp::Object<ManagedDto>& body) {
    return processBool(__FUNCTION__, [&](){
        if (!body || !body->manager_id) {
            throw ArgumentException("manager_id is required");
        }
        if (!isValidManagerId(body->manager_id)) {
            throw ArgumentException("manager_id must be 1-" + std::to_string(MaxManagerIdLength) + " characters long");
        }
        if (body->managed == nullptr) {
            throw ArgumentException("managed is required");
        }
        bool managed = *body->managed;

        std::lock_guard<std::mutex> lock(mutex_);
        managed_.active = managed;
        managed_.managerId = managed ? std::string(body->manager_id) : std::string();
        managedSetAt_ = std::chrono::steady_clock::now();
        return true;
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ManagerController::getControlled() {
    return process(__FUNCTION__, [&](){
        auto flag = controlledState_->get();
        auto dto = ControlledDto::createShared();
        dto->controlled = flag.active;
        dto->manager_id = flag.active ? oatpp::String(flag.managerId) : nullptr;
        return createDtoResponse(Status::CODE_200, dto);
    });
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ManagerController::setControlled(const oatpp::Object<ControlledDto>& body) {
    return processBool(__FUNCTION__, [&](){
        if (!body || !body->manager_id) {
            throw ArgumentException("manager_id is required");
        }
        if (!isValidManagerId(body->manager_id)) {
            throw ArgumentException("manager_id must be 1-" + std::to_string(MaxManagerIdLength) + " characters long");
        }
        if (body->controlled == nullptr) {
            throw ArgumentException("controlled is required");
        }
        bool controlled = *body->controlled;

        controlledState_->set(body->manager_id, controlled);
        return true;
    });
}
