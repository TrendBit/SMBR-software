#include "BasicCommands.hpp"
#include "SMBR/CallAtStartup.hpp"
#include <Poco/Thread.h>
#include <Poco/String.h>
#include <iostream>

WaitCommand::WaitCommand(Block::Ptr block, ParseContext::Ptr pctx){
    line = block->line.lineNumber();
    timeMs = block->line.argAsInt(0, 0, 24*3600 * 1000);
}

void WaitCommand::run(RunContext::Ptr rctx){
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
            {{"milliseconds", "Time to wait in milliseconds"}}, 
            "Pauses execution for a specified time."
        );
        f.registerCommand(info, [](Block::Ptr block, ParseContext::Ptr ctx) {
            return std::make_shared<WaitCommand>(block, ctx);
        });
    }
    {
        CommandInfo info(
            "print", 
            {{"content", "Content to print (any arguments on the line will be concatenated)"}}, 
            "Print text to script output."
        );
        f.registerCommand(info, [](Block::Ptr block, ParseContext::Ptr ctx) {
            return std::make_shared<PrintCommand>(block, ctx);
        });    
    }
}




