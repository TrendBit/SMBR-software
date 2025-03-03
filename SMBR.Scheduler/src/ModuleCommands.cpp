#include "ModuleCommands.hpp"

#include <SMBR/Exceptions.hpp>
#include <SMBR/Interpreter.hpp>
#include <SMBR/Script.hpp>
#include <SMBR/Parser.hpp>

#include <fstream>

void registerModuleBlocks(CommandFactory & f){

    {
        CommandInfo info(
            "illumination",
            {
                {"i0", "Intensity of channel 0"},
                {"i1", "Intensity of channel 1"},
                {"i2", "Intensity of channel 2"},
                {"i3", "Intensity of channel 3"},
            },
            "Illumination command"
        );
        f.registerCommand(info, [](Block::Ptr block, ParseContext::Ptr ctx) {
            return std::make_shared<Scripting::IlluminationCommand>(block, ctx);
        });
    }

    {
        CommandInfo info(
            "temperature",
            {
                {"temperature", "Target temperature"},
            },
            "Heating/Cooling command"
        );
        f.registerCommand(info, [](Block::Ptr block, ParseContext::Ptr ctx) {
            return std::make_shared<Scripting::HeaterCommand>(block, ctx);
        });
    }

    {
        CommandInfo info(
            "mix", 
            {
                {"time", "Time to mix"},
            },
            "Mixing command"
        );
        f.registerCommand(info, [](Block::Ptr block, ParseContext::Ptr ctx) {
            return std::make_shared<Scripting::MixerCommand>(block, ctx);
        });
    }

    {
        CommandInfo info(
            "pump",
            {
                {"speed", "Speed of the pump"},
            },
            "Pump command"
        );
        f.registerCommand(info, [](Block::Ptr block, ParseContext::Ptr ctx) {
            return std::make_shared<Scripting::PumpCommand>(block, ctx);
        });
    }

    {
        CommandInfo info(
            "air",
            {
                {"speed", "Speed of the aerator"},
            },
            "Aerator command"
        );
        f.registerCommand(info, [](Block::Ptr block, ParseContext::Ptr ctx) {
            return std::make_shared<Scripting::AeratorCommand>(block, ctx);
        });
    }

    {
        CommandInfo info(
            "display",
            {
                {"message", "Message to display", true},
            },
            "Display message on OLED unit of the device"
        );
        f.registerCommand(info, [](Block::Ptr block, ParseContext::Ptr ctx) {
            return std::make_shared<Scripting::DisplayCommand>(block, ctx);
        });
    }
}
