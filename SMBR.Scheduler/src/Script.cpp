#include "SMBR/Script.hpp"
#include <stdexcept>
#include <Poco/String.h>
#include <Poco/NumberParser.h>
#include <sstream>


ScriptLine::ScriptLine(){

}

ScriptLine::ScriptLine(int ln, const std::string & lc) : lineNumber_(ln), lineContent_(lc) {
    std::istringstream iss(lineContent_);
    
    std::string token;
    while (iss >> token) {
        std::string tok = Poco::trim(token);
        if (!tok.empty()){
            tokens_.push_back(tok);
        }
    }
}

std::string ScriptLine::command() const { 
    if (tokens_.empty()){
        throw std::runtime_error("No tokens in line " + std::to_string(lineNumber_));
    }
    return tokens_[0];
}

std::string ScriptLine::arg(int argNumber) const {
    if (argNumber + 1 >= tokens_.size()) {
        throw std::runtime_error("Not enough arguments in line " + std::to_string(lineNumber_));
    }
    return tokens_[argNumber + 1];
}

float ScriptLine::argAsFloat(int argNumber, float minValue, float maxValue) const {
    float val = Poco::NumberParser::parseFloat(arg(argNumber));
    if (val < minValue || val > maxValue){
        throw std::runtime_error("Value '" + std::to_string(val) + "' out of range [" + std::to_string(minValue) + ", " + std::to_string(maxValue) + "] in line " + std::to_string(lineNumber_));
    }
    return val;
}
int ScriptLine::argAsInt(int argNumber, int minValue, int maxValue) const {
    int val = Poco::NumberParser::parse(arg(argNumber));
    if (val < minValue || val > maxValue){
        throw std::runtime_error("Value '" + std::to_string(val) + "' out of range [" + std::to_string(minValue) + ", " + std::to_string(maxValue) + "] in line " + std::to_string(lineNumber_));
    }
    return val;
}

bool ScriptLine::empty() const {
    return tokens_.empty();   
}

bool ScriptLine::noArgs() const {
    return tokens_.size() <= 1;
}
int ScriptLine::args() const {
    return tokens_.size() - 1;
}

int ScriptLine::lineNumber() const {
    return lineNumber_;
}

void Script::addNamedBlock(const std::string & name, std::shared_ptr<Block> block){
    blocks[name] = block;
}


std::shared_ptr<Block> Script::getNamedBlock(const std::string & name) const {
    auto it = blocks.find(name);
    if (it == blocks.end()){
        throw std::runtime_error("Block " + name + " not found");
    }
    return it->second;
}

std::vector <std::string> Script::lines() const {
    return lines_;
}
void Script::addLine(const std::string & line){
    lines_.push_back(line);
}

std::set <std::string> Script::blockNames() const {
    std::set <std::string> names;
    for (auto & block : blocks){
        names.insert(block.first);
    }
    return names;
}

