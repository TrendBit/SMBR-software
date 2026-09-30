#include "ManagerController.hpp"

ManagerController::ManagerController(const std::shared_ptr<oatpp::web::mime::ContentMappers>& apiContentMappers,
                                     std::shared_ptr<ISystemModule> systemModule)
    : SMBRControllerBase(apiContentMappers, systemModule)
{}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> ManagerController::getManaged() {
    return process(__FUNCTION__, [&](){
        Flag flag;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            flag = managed_;
        }
        auto dto = ManagedDto::createShared();
        dto->managed = flag.active;
        dto->manager_id = flag.active ? oatpp::String(flag.managerId) : nullptr;
        return createDtoResponse(Status::CODE_200, dto);
    });
}
