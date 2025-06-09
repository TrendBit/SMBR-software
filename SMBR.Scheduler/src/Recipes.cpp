#include "SMBR/Recipes.hpp"
#include <stdexcept>
#include <Poco/Path.h>
#include <Poco/Glob.h>
#include <Poco/File.h>
#include <Poco/String.h>
#include <Poco/StringTokenizer.h>
#include <fstream>
#include <sstream>
#include <iostream>



Recipes::Recipes(std::string folder, std::string referenceFolder) : folder_(folder) {

    Poco::File path(folder);
    path.createDirectories();

    //copy content of reference folder to folder, overwrite existing files (recursive copy)
    Poco::File reference(referenceFolder);
    if (reference.exists() || reference.isDirectory()){
        system(("cp -r " + referenceFolder + "/* " + folder).c_str());
    }
    
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

static void checkName(std::string name){
    //name can contain only letters, numbers, and underscores

    if (name.empty()){
        throw std::runtime_error("Empty recipe name");
    }

    for (auto c : name){
        if (!std::isalnum(c) && c != '_' && c != '|'){
            throw std::runtime_error("Invalid character in recipe name: " + name);
        }
    }
}


static std::string pathToName(std::string baseFolder, std::string completePath){
    Poco::Path path(completePath);
    std::string name = path.getBaseName();
    if (name.empty()){
        throw std::runtime_error("Empty recipe name from path: " + completePath);
    }
    if (path.getExtension() != "recipe"){
        throw std::runtime_error("Invalid recipe file extension: " + completePath);
    }
    if (completePath.find(baseFolder) != 0){
        throw std::runtime_error("Recipe file is not in the base folder: " + completePath);
    }

    std::string prefix = completePath.substr(baseFolder.size());
    Poco::StringTokenizer tokenizer(prefix, "/");
    std::string finalName;
    for (int tok = 0; tok + 1 < tokenizer.count(); tok++){      
        finalName += tokenizer[tok] + "|";
    }
    finalName += name;

    //std::cout << "FROM " << completePath << " base " << baseFolder << " -> " << finalName << std::endl; 

    return finalName;
}

static std::string nameToPath(std::string baseFolder, std::string name){
    if (name.empty()){
        throw std::runtime_error("Empty recipe name");
    }
    checkName(name);
    std::string nameReplaced = baseFolder + Poco::replace(name, "|", "/") + ".recipe";
    
    Poco::Path ppath(nameReplaced);
    Poco::File(ppath.makeParent().toString()).createDirectories();
    return nameReplaced;
}

void Recipes::replaceRecipe(const ScriptInfo & script) {

    std::lock_guard <std::mutex> lock(mutex_);
    

    checkName(script.name);
    std::string path = nameToPath(folder_, script.name); 

    

    std::ofstream file(path);
    if (!file){
        throw std::runtime_error("Failed to open file for writing: " + path);
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
 
    checkName(name);
    std::string path = nameToPath(folder_, name); 
    

    if (std::remove(path.c_str()) != 0){
        throw std::runtime_error("Failed to delete file: " + path);
    }

    recipes_.erase(it);
}

static void loadRecursive(std::map <std::string, ScriptInfo> & recipes_, std::string baseFolder, std::string currentFolder){
    //std::cout << "LoadRecursive: " << currentFolder << std::endl;
    Poco::Path path(currentFolder);
    std::set <std::string> files;
    Poco::Glob::glob(path.toString() + "/*", files);
    for (auto & file : files){

        Poco::File ff(file);
        if (ff.isDirectory()){
            loadRecursive(recipes_, baseFolder, file);
        } else if (file.find(".recipe") == std::string::npos){
            continue; 
        } else {

            std::ifstream f(file);
            if (!f){
                throw std::runtime_error("Failed to open file for reading: " + file);
            }
            std::stringstream buffer;
            buffer << f.rdbuf();
            ScriptInfo s;

            s.name = pathToName(baseFolder, file);

            s.content = buffer.str();
            recipes_[s.name] = s;
        }
    }
}

void Recipes::refresh() {
    std::lock_guard <std::mutex> lock(mutex_);
    recipes_.clear();
    loadRecursive(recipes_, folder_, folder_);    
}



