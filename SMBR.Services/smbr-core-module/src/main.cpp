#include "main.hpp"
#include "core_module.hpp"


int main(int argc, char* argv[]) {
    try {
        //hack
        system("modprobe i2c-dev");

        if (argc > 1) {
            std::string arg(argv[1]);

            if (arg == "--probe") {
                I2C_bus i2c("/dev/i2c-1");
                Core_module core(&i2c);
                core.Probe();
                return 0;
            }

            // Handle info arguments without I2C
            RPi_host rpi;

            if (arg == "--sid") {
                std::cout << "0x" << std::hex << std::setfill('0') << std::setw(4)
                         << rpi.Short_ID() << std::endl;
                return 0;
            }

            if (arg == "--host") {
                std::cout << rpi.Hostname() << std::endl;
                return 0;
            }

            if (arg == "--ip") {
                auto ip = rpi.IP_address();
                if (ip) {
                    for (size_t i = 0; i < ip->size(); i++) {
                        std::cout << static_cast<int>((*ip)[i]);
                        if (i < ip->size() - 1) std::cout << ".";
                    }
                    std::cout << std::endl;
                }
                return 0;
            }

            if (arg == "--serial") {
                std::cout << "0x" << std::hex << std::uppercase << std::setfill('0')
                         << std::setw(8) << rpi.Serial_number() << std::endl;
                return 0;
            }
        }

        // Default behavior
        I2C_bus i2c("/dev/i2c-1");
        Core_module core(&i2c);
        core.Run();

    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    return 0;
}
