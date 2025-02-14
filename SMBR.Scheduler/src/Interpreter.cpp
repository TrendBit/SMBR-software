#include "SMBR/Interpreter.hpp"
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <stack>
#include <memory>
#include <list>
#include <BasicCommands.hpp>
#include <ModuleCommands.hpp>
#include <iostream>

Stack::Stack(){}
Stack::Stack(std::function <void(const Data &)> updateCb) : updateCb(updateCb){

}

std::vector <int> Stack::stack() const {
    std::lock_guard <std::mutex> lock(mtx);
    return stack_;
}

void Stack::push(int line) {
    update([&](){
        stack_.push_back(line);
    });
}
int Stack::pop() {
    int v;
    update([&](){
        if (stack_.empty()) {
            throw std::runtime_error("Stack is empty.");
        }
        v = stack_.back();
        stack_.pop_back();
    });
    return v;
}

void Stack::updateTop(int line){
    update([&](){
        if (stack_.empty()) {
            throw std::runtime_error("Stack is empty.");
        }
        stack_.back() = line;    
    });
}

void Stack::update(std::function <void()> internal){
    std::vector <int> oldStack = stack_;
    std::vector <int> newStack;
    {
        std::lock_guard <std::mutex> lock(mtx);
        internal();
        newStack = stack_;
    }
    
    if (oldStack != newStack){
        updateCb(newStack);
    }
}


CommandFactory & CommandFactory::instance(){
    static CommandFactory factory;
    return factory;
}


void CommandFactory::registerCommand(CommandInfo commandInfo, std::function <ICommand::Ptr(Block::Ptr, ParseContext::Ptr)> factoryFnc){
    factories[commandInfo.name] = factoryFnc;
    documentation[commandInfo.name] = commandInfo;
}

ICommand::Ptr CommandFactory::create(Block::Ptr block, ParseContext::Ptr ctx){

    init();

    auto it = factories.find(block->line.command());
    if (it == factories.end()){

        for (auto bn : ctx->script.blockNames()){
            if (bn == block->line.command()){
                return std::make_shared<NamedBlockCommand>(bn, ctx);
            }
        }
        
        throw std::runtime_error("Command " + block->line.command() + " not found");
    }
    return it->second(block, ctx);
}

ICommand::Ptr Interpreter::build(Block::Ptr block, ParseContext::Ptr pctx){
    return CommandFactory::instance().create(block, pctx);
    
}

void CommandFactory::init(){
    if (factories.empty()){
        registerBasicBlocks(*this);
        registerModuleBlocks(*this);
    }
}

void CommandFactory::createDocumentation(std::ostream & os){
    init();

/*
### `wait <milliseconds>`
Pauses execution for a specified time.
- **Syntax**: `wait 1000` (waits 1000 ms)

*/ 


    //create markdown language docs
    for (auto & f : documentation){
           os << "### `" << f.first;
            for (auto & a : f.second.arguments){
                if (a.required){
                    os << " <" << a.name << ">";
                } else {
                    os << " [" << a.name << "]";
                }
            }
            os << "`" << std::endl;

           os << f.second.description << std::endl;     

            os << "#### Arguments" << std::endl;
            os << "| Name | Description | Required |" << std::endl;
            os << "|------|-------------|----------|" << std::endl;
            for (auto & a : f.second.arguments){
                os << "| `" << a.name << "` | " << a.description << " | " << (a.required ? "Yes" : "No") << " |" << std::endl;
            }

            os << std::endl;
    }

}
