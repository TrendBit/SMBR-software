#pragma once

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <stack>
#include <SMBR/Script.hpp>
#include <SMBR/ISystemModule.hpp>



class Stack {
    public:
        typedef std::shared_ptr<Stack> Ptr;
        typedef std::vector <int> Data;

        Stack();
        Stack(std::function <void(const Data &)> updateCb);
        
        std::vector <int> stack() const ;

        void push(int line);
        int pop();

        void updateTop(int line);
    private:
        void update(std::function <void()> internal);
    private:
        std::vector <int> stack_;
        std::function <void(const Data &)> updateCb;
        mutable std::mutex mtx;
};


class ParseContext {
    public:
        typedef std::shared_ptr<ParseContext> Ptr;
        const Script & script;
        int stackDepth = 0;
    public:
        ParseContext(Script & script) : script(script){
            
        }
};

class RunContext {
    public:
        typedef std::shared_ptr<RunContext> Ptr;
        Stack::Ptr stack;
        ISystemModule::Ptr module;

        std::function <bool()> stopCb;
        std::function <void(const std::string &)> printCb;
    public:
        RunContext(Stack::Ptr stack,  ISystemModule::Ptr module) : stack(stack), module(module) {
            
        }
        void checkRunning() {
            if (stopCb) {
                bool ret = stopCb();
                if (ret) {
                    throw std::runtime_error("Stopped");
                }
            }
        }
};

class ICommand {
    public:
        typedef std::shared_ptr<ICommand> Ptr;
        virtual ~ICommand() = default;
        virtual void run(RunContext::Ptr runCtx) = 0;
        virtual std::string name() const = 0;
};

struct CommandArgument {
    std::string name;
    std::string description;
    bool required = true;

    CommandArgument(const std::string & name, const std::string & description, bool required = true) 
        : name(name), description(description), required(required) {
        
    }
};

struct CommandInfo {
    std::string name;
    std::vector <CommandArgument> arguments;
    std::string description;
    CommandInfo(std::string name = "", std::vector <CommandArgument> arguments = {}, std::string  description = std::string()) : name(name), arguments(arguments), description(description) {
        
    }
};

class CommandFactory {
    public:
        static CommandFactory & instance();

        void registerCommand(CommandInfo commandInfo, std::function <ICommand::Ptr(Block::Ptr, ParseContext::Ptr)> factoryFnc);

        ICommand::Ptr create(Block::Ptr block, ParseContext::Ptr ctx);

        void createDocumentation(std::ostream & os);
    private:
        void init();
    private:
        static CommandFactory i;
        std::unordered_map <std::string, std::function<ICommand::Ptr(Block::Ptr, ParseContext::Ptr)> > factories;
        std::unordered_map <std::string, CommandInfo> documentation;
};

class Interpreter {
    
    public:    

        static ICommand::Ptr build(Block::Ptr block, ParseContext::Ptr pctx);
        
    private:
        
};
