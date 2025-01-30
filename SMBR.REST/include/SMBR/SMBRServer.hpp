#include <SMBR/ISystemModule.hpp>
#include <memory>

class SMBRServer {
public:
    SMBRServer(std::shared_ptr<ISystemModule> systemModule);
    ~SMBRServer();

    void run();
private:
    std::shared_ptr<ISystemModule> systemModule;
};