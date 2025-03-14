#include <SMBR/Interpreter.hpp>

class NestedBlockCommand : public ICommand {
    public:
        NestedBlockCommand(Block::Ptr block, ParseContext::Ptr pctx){

            for (auto & nestedBlock : block->nestedBlocks) {
                commands.push_back(Interpreter::build(nestedBlock, pctx));
            }
            if (!block->nestedBlocks.empty()) {
                firstLine = block->nestedBlocks.front()->line.lineNumber();
            }
        }

        void run(RunContext::Ptr rctx) override {
            rctx->checkRunning();

            if (commands.empty()) return;

            rctx->stack->push(firstLine);
            for (auto & cmd : commands) {
                rctx->checkRunning();
                cmd->run(rctx);
            }
            rctx->stack->pop();
        }
    private:
        int firstLine;
        std::vector <ICommand::Ptr> commands;

};

class LoopCommand : public ICommand {
    public:
        LoopCommand(Block::Ptr loopBlock, ParseContext::Ptr pctx){
            line = loopBlock->line.lineNumber();
            infinity = loopBlock->line.args() == 0;
            if (!infinity){
                repeat = loopBlock->line.argAsInt(0, 0, 1000000);
            }
            nested = std::make_shared<NestedBlockCommand>(loopBlock, pctx);
        }
        void run(RunContext::Ptr rctx) override {

            rctx->stack->push(line);

            if (infinity){
                while (true) {
                    rctx->checkRunning();
                    nested->run(rctx);
                }
            } else {
                for (int i = 0; i < repeat; i++) {
                    rctx->checkRunning();
                    nested->run(rctx);
                }
            }

            rctx->stack->pop();
        }
    private:
        int line = 0;
        bool infinity = false;
        int repeat = 0;
        NestedBlockCommand::Ptr nested;
};

class NamedBlockCommand : public ICommand {
    public:
        NamedBlockCommand(std::string name, ParseContext::Ptr pctx){
            auto block = pctx->script.getNamedBlock(name);
            line = block->line.lineNumber();
            nested = std::make_shared<NestedBlockCommand>(block, pctx);
        }
        void run(RunContext::Ptr rctx) override {
            rctx->stack->push(line);
            nested->run(rctx);
            rctx->stack->pop();
        }
    private:
        NestedBlockCommand::Ptr nested;
        int line;
};

class WaitCommand : public ICommand {
    public:
        WaitCommand(Block::Ptr block, ParseContext::Ptr pctx);
        void run(RunContext::Ptr rctx) override;
    private:
        int line;
        long timeMs;
};

class PrintCommand : public ICommand {
    public:
        PrintCommand(Block::Ptr block, ParseContext::Ptr pctx);
        void run(RunContext::Ptr rctx) override;
    private:
        int line;
        std::string content;
};

void registerBasicBlocks(CommandFactory & f);
