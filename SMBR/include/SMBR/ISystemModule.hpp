#pragma once
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <future>
#include <ostream>
#include <set>

#include "SMBR.hpp"
#include "ISensorModule.hpp"
#include "ICoreModule.hpp"
#include "IControlModule.hpp"
#include "ICommonModule.hpp"


class ISystemModule {
    public:
        typedef std::shared_ptr <ISystemModule> Ptr;
        
        virtual ~ISystemModule() = default;

        typedef std::vector <ModuleID> AvailableModules;
        virtual std::future <AvailableModules> getAvailableModules() = 0;

        virtual std::shared_ptr <ISensorModule> sensorModule() = 0;
        virtual std::shared_ptr <IControlModule> controlModule() = 0;
        virtual std::shared_ptr <ICoreModule> coreModule() = 0;

        virtual std::shared_ptr <ICommonModule> commonModule(ModuleID module) = 0;

        virtual std::set <ModuleID> existing() = 0;

};

std::ostream& operator<<(std::ostream& os, const ISystemModule::AvailableModules& modules);

