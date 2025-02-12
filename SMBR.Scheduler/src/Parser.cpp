#include "SMBR/Parser.hpp"
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <stack>
#include <memory>
#include <list>
#include <iostream>
#include <Poco/String.h>

Script Parser::parse(const std::string & scriptFile){
    
    std::ifstream file(scriptFile);
    if (!file) {
        throw std::runtime_error("Cannot open file " + scriptFile);
    }

    std::string line;
    int lineNumber = 0;

    std::list <Block::Ptr> blockStack;



    Script script;
    script.addLine(scriptFile);

    while (getline(file, line)) {
        lineNumber++;
        
        script.addLine(line);

        std::string trimmedLine = Poco::trim(line);

        if (trimmedLine.empty()) continue;

        int indent = 0;
        while (indent < line.size() && line[indent] == ' ') indent++;

        std::cout << "line: " << lineNumber << " indent: " << indent << " blockStack: " << blockStack.size() << " content: " << line << std::endl;

        if (line.back() == ':' && indent == 0) { // Block definition
            
            std::string name = line.substr(0, line.size() - 1);

            std::cout << "Block definition " << name << std::endl;

            Block::Ptr namedBlock = std::make_shared<Block>();
            namedBlock->line = ScriptLine(lineNumber, name);
            namedBlock->indent = indent;
        
            blockStack.clear();
            blockStack.push_back(namedBlock);

            script.addNamedBlock(name, namedBlock);

        } else if (!blockStack.empty()) {
            Block::Ptr lastBlock = blockStack.back();
            if (indent == lastBlock->indent + 4) { 
                // Nested block
                Block::Ptr nestedBlock = std::make_shared<Block>();
                nestedBlock->line = ScriptLine(lineNumber, line.substr(indent));
                nestedBlock->indent = indent;
                lastBlock->nestedBlocks.push_back(nestedBlock);
                blockStack.push_back(nestedBlock);
            } else if (indent == lastBlock->indent) {

                if (blockStack.size() < 2){
                    throw std::runtime_error("Syntax error on line " + std::to_string(lineNumber) + ": Incorrect indentation A.");
                }

                blockStack.pop_back();
                Block::Ptr parentBlock = blockStack.back();
                Block::Ptr nestedBlock = std::make_shared<Block>();
                nestedBlock->line = ScriptLine(lineNumber, line.substr(indent));
                nestedBlock->indent = indent;
                parentBlock->nestedBlocks.push_back(nestedBlock);

                blockStack.push_back(nestedBlock);
            } else if (indent < lastBlock->indent) {
                while (!blockStack.empty() && indent < blockStack.back()->indent) {
                    blockStack.pop_back();
                }
                if (blockStack.empty()) {
                    throw std::runtime_error("Syntax error on line " + std::to_string(lineNumber) + ": Incorrect indentation B.");
                }
                Block::Ptr parentBlock = blockStack.back();
                Block::Ptr siblingBlock = std::make_shared<Block>();
                siblingBlock->line = ScriptLine(lineNumber, line.substr(indent));
                siblingBlock->indent = indent;
                parentBlock->nestedBlocks.push_back(siblingBlock);
                blockStack.push_back(siblingBlock);
            } else {
                throw std::runtime_error("Syntax error on line " + std::to_string(lineNumber) + ": Incorrect indentation " + std::to_string(indent) + " lastBlock: " + std::to_string(lastBlock->indent));
            } 
        } else {
            throw std::runtime_error("Syntax error on line " + std::to_string(lineNumber) + ": Command outside of any block.");
        }
    }
    return script;
}
