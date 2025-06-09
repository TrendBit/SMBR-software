#pragma once

#include <map>
#include <mutex>
#include <string>
#include "SMBR/IRecipes.hpp"

class Recipes : public IRecipes {
    
    public:    

        
        Recipes(std::string folder, std::string referenceFolder);
        

        std::vector <std::string> getRecipeNames() const override;
        ScriptInfo getRecipeContent(const std::string & name) const override;

        void reload() override;

        void replaceRecipe(const ScriptInfo & script) override;
        void deleteRecipe(const std::string & name) override;
    private:
        void refresh();
    private:
        mutable std::mutex mutex_;
        std::string folder_;
        std::map <std::string, ScriptInfo> recipes_;
};

