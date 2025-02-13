#pragma once

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <set>

class ScriptLine {
    
    public:    

        
        ScriptLine();
        ScriptLine(int lineNumber, const std::string & lineContent);

        //first token
        std::string command() const ;
        //argNumber is counted from 0 (command does not count as arg)
        std::string arg(int argNumber) const;

        float argAsFloat(int argNumber, float minValue, float maxValue) const;
        int argAsInt(int argNumber, int minValue, int maxValue) const;
        //empty line
        bool empty() const;
        //just command without args
        bool noArgs() const;
        int args() const;

        int lineNumber() const;
    private:
        int lineNumber_ = 0;
        std::string lineContent_;
        std::vector <std::string> tokens_;
        
};


struct Block {
    typedef std::shared_ptr<Block> Ptr;
    
    int indent = 0;
    ScriptLine line;
    std::vector <Block::Ptr> nestedBlocks;
    
};

class Script {
    
    public:
        void addNamedBlock(const std::string & name, Block::Ptr block);
        std::shared_ptr<Block> getNamedBlock(const std::string & name) const;

        std::vector <std::string> lines() const;
        void addLine(const std::string & line);

        std::set <std::string> blockNames() const;
    private:
        std::unordered_map<std::string, Block::Ptr > blocks;
        std::vector <std::string> lines_;
};

