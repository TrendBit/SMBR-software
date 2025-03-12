#pragma once

#include <memory>
#include <string>
#include <deque>
#include <ostream>
#include <Poco/Timestamp.h>
#include <vector>
#include "IScheduler.hpp"


class IRecipes {
    public:
        typedef std::shared_ptr <IRecipes> Ptr;

        virtual ~IRecipes() = default;

        virtual std::vector <std::string> getRecipeNames() const  = 0;
        virtual ScriptInfo getRecipeContent(const std::string & name) const = 0;

        virtual void reload() = 0;

        virtual void replaceRecipe(const ScriptInfo & script) = 0;
        virtual void deleteRecipe(const std::string & name) = 0;
};

