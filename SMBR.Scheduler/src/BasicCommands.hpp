#include <SMBR/Interpreter.hpp>
#include <iostream>
#include <SMBR/Log.hpp>

class NestedBlockCommand : public ICommand {
    public:
        NestedBlockCommand(Block::Ptr block, ParseContext::Ptr pctx);

        void run(RunContext::Ptr rctx) override;

        std::string name() const override {
            return "nested (line " + std::to_string(firstLine) + ")";
        }

    private:
        int firstLine;
        std::vector <ICommand::Ptr> commands;

};

class LoopCommand : public ICommand {
    public:
        LoopCommand(Block::Ptr loopBlock, ParseContext::Ptr pctx);
        void run(RunContext::Ptr rctx) override;

        std::string name() const override {
            return "loop (line " + std::to_string(line) + ")";
        }
    private:
        int line = 0;
        bool infinity = false;
        int repeat = 0;
        NestedBlockCommand::Ptr nested;
};

class NamedBlockCommand : public ICommand {
    public:
        NamedBlockCommand(std::string name, ParseContext::Ptr pctx);
        void run(RunContext::Ptr rctx) override;
        std::string name() const override {
            return n + " (line " + std::to_string(line) + ")";
        }
    private:
        NestedBlockCommand::Ptr nested;
        std::string n;
        int line;
};

class WaitCommand : public ICommand {
    public:
        WaitCommand(Block::Ptr block, ParseContext::Ptr pctx);
        void run(RunContext::Ptr rctx) override;
        std::string name() const override {
            return "wait" + std::to_string(timeMs) + "ms (line " + std::to_string(line) + ")";
        }
    private:
        int line;
        long timeMs;
};

class PrintCommand : public ICommand {
    public:
        PrintCommand(Block::Ptr block, ParseContext::Ptr pctx);
        void run(RunContext::Ptr rctx) override;
        std::string name() const override {
            return "print (line " + std::to_string(line) + ")";
        }
    private:
        int line;
        std::string content;
};

void registerBasicBlocks(CommandFactory & f);
