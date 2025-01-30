#pragma once
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <future>
#include <ostream>

#include "ISensorModule.hpp"
#include "ICoreModule.hpp"
#include "IControlModule.hpp"
#include "ICommonModule.hpp"

enum Modules {
            Core = 1,
            Control = 2,
            Sensor = 3
        };

class ISystemModule {
    public:
        typedef std::shared_ptr <ISystemModule> Ptr;
        
        virtual ~ISystemModule() = default;

        typedef std::vector <Modules> AvailableModules;
        virtual std::future <AvailableModules> getAvailableModules() = 0;

        virtual std::shared_ptr <ISensorModule> sensorModule() = 0;
        virtual std::shared_ptr <IControlModule> controlModule() = 0;
        virtual std::shared_ptr <ICoreModule> coreModule() = 0;

        virtual std::shared_ptr <ICommonModule> commonModule(Modules module) = 0;

};

std::ostream& operator<<(std::ostream& os, const Modules& module);
std::ostream& operator<<(std::ostream& os, const ISystemModule::AvailableModules& modules);

