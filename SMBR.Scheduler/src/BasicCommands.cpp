#include "BasicCommands.hpp"
#include "SMBR/CallAtStartup.hpp"
#include <Poco/Thread.h>
#include <Poco/String.h>
#include <iostream>

NestedBlockCommand::NestedBlockCommand(Block::Ptr block, ParseContext::Ptr pctx){

    for (auto & nestedBlock : block->nestedBlocks) {
        commands.push_back(Interpreter::build(nestedBlock, pctx));
    }
    if (!block->nestedBlocks.empty()) {
        firstLine = block->nestedBlocks.front()->line.lineNumber();
    }
}

void NestedBlockCommand::run(RunContext::Ptr rctx){
    rctx->checkRunning();

    if (commands.empty()) return;

    rctx->stack->push(firstLine);

    int cmds = commands.size();   
    int counter = 0;
    LTRACE("Scheduler") << "run " << name() << LE;
    for (auto & cmd : commands) {
        rctx->checkRunning();
        LTRACE("Scheduler") << "run " << name() << " " << "cmd " << ++counter << "/" << cmds << LE;
        cmd->run(rctx);
        LTRACE("Scheduler") << "run " << name() << " " << "cmd " << counter << "/" << cmds << LE;
    }
    LTRACE("Scheduler") << "run " << name() << " done" << LE;
    rctx->stack->pop();
}

LoopCommand::LoopCommand(Block::Ptr loopBlock, ParseContext::Ptr pctx){
    line = loopBlock->line.lineNumber();
    infinity = loopBlock->line.args() == 0;
    if (!infinity){
        repeat = loopBlock->line.argAsInt(0, 0, 1000000);
    }
    nested = std::make_shared<NestedBlockCommand>(loopBlock, pctx);
}
void LoopCommand::run(RunContext::Ptr rctx) {
    LTRACE("Scheduler") << "run " << name() << LE;
    rctx->stack->push(line);

    if (infinity){
        while (true) {
            rctx->checkRunning();
            nested->run(rctx);
        }
    } else {
        for (int i = 0; i < repeat; i++) {
            LTRACE("Scheduler") << "run " << name() << " " << "iteration " << i+1 << "/" << repeat << LE;
            rctx->checkRunning();
            nested->run(rctx);
            LTRACE("Scheduler") << "run " << name() << " " << "iteration " << i+1 << "/" << repeat << " done" << LE;
        }
    }

    rctx->stack->pop();
}

NamedBlockCommand::NamedBlockCommand(std::string name, ParseContext::Ptr pctx) : n(name){
    auto block = pctx->script.getNamedBlock(name);
    line = block->line.lineNumber();
    nested = std::make_shared<NestedBlockCommand>(block, pctx);
}
void NamedBlockCommand::run(RunContext::Ptr rctx) {
    LTRACE("Scheduler") << "run " << name() << LE;
    rctx->stack->push(line);
    nested->run(rctx);
    rctx->stack->pop();
}


WaitCommand::WaitCommand(Block::Ptr block, ParseContext::Ptr pctx){
    line = block->line.lineNumber();
    timeMs = block->line.argAsFloat(0, 0, 24*3600) * 1000;
}

void WaitCommand::run(RunContext::Ptr rctx){
    LTRACE("Scheduler") << "run " << name() << LE;
    rctx->stack->updateTop(line);

    long remainingSleep = timeMs;
    while (remainingSleep > 0) {
        rctx->checkRunning();
        long sleepTime = std::min(remainingSleep, 1000L);
        Poco::Thread::sleep(static_cast<int>(sleepTime));
        remainingSleep -= sleepTime;
    }
}

PrintCommand::PrintCommand(Block::Ptr block, ParseContext::Ptr pctx){
    line = block->line.lineNumber();
    for (int i = 0; i < block->line.args(); i++) {
        content += block->line.arg(i) + " ";
    }
    Poco::replaceInPlace(content, "\"", "");
}

void PrintCommand::run(RunContext::Ptr rctx){
    LTRACE("Scheduler") << "run " << name() << LE;
    rctx->stack->updateTop(line);
    std::cout << "PRINT: " << content << std::endl;
    if (rctx->printCb) {
        rctx->printCb(content);
    }
}

void registerBasicBlocks(CommandFactory & f){
    {
        CommandInfo info(
            "loop",
            {{"times", "Number of times to loop - infinity if not specified", false}},
            "Repeats the indented block of commands for the given number of iterations."
        );
        f.registerCommand(info, [](Block::Ptr block, ParseContext::Ptr ctx) {
            return std::make_shared<LoopCommand>(block, ctx);
        });
    }
    {
        CommandInfo info(
            "wait",
            {{"seconds", "Time to wait in seconds"}},
            "Pauses execution for a specified time."
        );
        f.registerCommand(info, [](Block::Ptr block, ParseContext::Ptr ctx) {
            return std::make_shared<WaitCommand>(block, ctx);
        });
    }
    {
        CommandInfo info(
            "print",
            {{"content", "Content to print, previous text will be overridden"}},
            "Print text to script output."
        );
        f.registerCommand(info, [](Block::Ptr block, ParseContext::Ptr ctx) {
            return std::make_shared<PrintCommand>(block, ctx);
        });
    }
}




