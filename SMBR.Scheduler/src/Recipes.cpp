#include "SMBR/Recipes.hpp"
#include <stdexcept>
#include <Poco/Path.h>
#include <Poco/Glob.h>
#include <Poco/File.h>
#include <fstream>
#include <sstream>

Recipes::Recipes(std::string folder) : folder_(folder) {

    Poco::File path(folder);
    path.createDirectories();
    
    refresh();
}

std::vector <std::string> Recipes::getRecipeNames() const {
    std::lock_guard <std::mutex> lock(mutex_);
    std::vector <std::string> names;
    for (auto & [name, script] : recipes_){
        names.push_back(name);
    }
    return names;
}

ScriptInfo Recipes::getRecipeContent(const std::string & name) const {
    std::lock_guard <std::mutex> lock(mutex_);
    auto it = recipes_.find(name);
    if (it == recipes_.end()){
        throw std::runtime_error("Script not found: " + name);
    }
    return it->second;
}

void Recipes::reload() {
    refresh();
}

static std::string checkName(std::string name){
    //name can contain only letters, numbers, and underscores

    if (name.empty()){
        throw std::runtime_error("Empty recipe name");
    }

    for (auto c : name){
        if (!std::isalnum(c) && c != '_'){
            throw std::runtime_error("Invalid character in recipe name: " + name);
        }
    }
    return name;
}

void Recipes::replaceRecipe(const ScriptInfo & script) {

    std::lock_guard <std::mutex> lock(mutex_);
    

    //save to file
    Poco::Path path(folder_);
    path.append(checkName(script.name) + ".recipe");
    std::ofstream file(path.toString());
    if (!file){
        throw std::runtime_error("Failed to open file for writing: " + path.toString());
    }
    file << script.content;

    recipes_[script.name] = script;
}

void Recipes::deleteRecipe(const std::string & name) {
    std::lock_guard <std::mutex> lock(mutex_);
    auto it = recipes_.find(name);
    if (it == recipes_.end()){
        throw std::runtime_error("Recipe not found: " + name);
    }
 
    //delete file
    Poco::Path path(folder_);
    path.append(checkName(name) + ".recipe");
    if (std::remove(path.toString().c_str()) != 0){
        throw std::runtime_error("Failed to delete file: " + path.toString());
    }

       recipes_.erase(it);
}

void Recipes::refresh() {
    std::lock_guard <std::mutex> lock(mutex_);
    recipes_.clear();
    Poco::Path path(folder_);
    std::set <std::string> files;
    Poco::Glob::glob(path.toString() + "/*.recipe", files);
    for (auto & file : files){
        std::ifstream f(file);
        if (!f){
            throw std::runtime_error("Failed to open file for reading: " + file);
        }
        std::stringstream buffer;
        buffer << f.rdbuf();
        ScriptInfo s;
        s.name = Poco::Path(file).getBaseName();
        s.content = buffer.str();
        recipes_[s.name] = s;
    }
}



