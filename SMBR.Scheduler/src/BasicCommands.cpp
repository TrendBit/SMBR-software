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
    f.registerCommand("loop", [](Block::Ptr block, ParseContext::Ptr ctx) {
        return std::make_shared<LoopCommand>(block, ctx);
    });    
    f.registerCommand("wait", [](Block::Ptr block, ParseContext::Ptr ctx) {
        return std::make_shared<WaitCommand>(block, ctx);
    });
    f.registerCommand("print", [](Block::Ptr block, ParseContext::Ptr ctx) {
        return std::make_shared<PrintCommand>(block, ctx);
    });
}




