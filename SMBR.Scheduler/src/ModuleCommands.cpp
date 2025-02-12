#include "ModuleCommands.hpp"

#include <SMBR/Exceptions.hpp>
#include <SMBR/Interpreter.hpp>
#include <SMBR/Script.hpp>
#include <SMBR/Parser.hpp>

#include <fstream>

void registerModuleBlocks(CommandFactory & f){
    f.registerCommand("illumination", [](Block::Ptr block, ParseContext::Ptr ctx) {
        return std::make_shared<Scripting::IlluminationCommand>(block, ctx);
    });
    f.registerCommand("heat", [](Block::Ptr block, ParseContext::Ptr ctx) {
        return std::make_shared<Scripting::HeaterCommand>(block, ctx);
    });
    f.registerCommand("mix", [](Block::Ptr block, ParseContext::Ptr ctx) {
        return std::make_shared<Scripting::MixerCommand>(block, ctx);
    });
    f.registerCommand("pump", [](Block::Ptr block, ParseContext::Ptr ctx) {
        return std::make_shared<Scripting::PumpCommand>(block, ctx);
    });
    f.registerCommand("air", [](Block::Ptr block, ParseContext::Ptr ctx) {
        return std::make_shared<Scripting::AeratorCommand>(block, ctx);
    });
}
