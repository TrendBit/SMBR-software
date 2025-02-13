#pragma once

#include "Script.hpp"
#include <string>
#include <istream>

class Parser {
    
    public:    

        static Script parseFile(const std::string & scriptFile);
        static Script parseContent(std::string name, const std::string & content);
        static Script parseStream(std::string name, std::istream & s);
        
};


