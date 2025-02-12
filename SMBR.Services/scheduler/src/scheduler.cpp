#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <stack>
#include <memory>
#include <list>
#include <stdexcept>
#include <functional>
#include <future>
#include <chrono>
#include <thread>

#include <SMBR/Script.hpp>
#include <SMBR/Parser.hpp>
#include <SMBR/Interpreter.hpp>

#include "SMBR/VirtualModulesFactory.hpp"
#include "SMBR/CanModulesFactory.hpp"


int main(int argc, char* argv[]) {
    try {
        if (argc < 2) {
            std::cerr << "Usage: " << argv[0] << " <script_file> [--debug] [--virtual]\n";
            return 1;
        }

        bool debugMode = false;    
        bool isVirtual = false;

        for (int a = 2; a < argc; a++){
            if (std::string(argv[a]) == "--debug") {
                debugMode = true;
            }
            if (std::string(argv[a]) == "--virtual") {
                isVirtual = true;
            }
        }

        std::shared_ptr<ISystemModule> systemModule = nullptr;


        if (isVirtual){
            systemModule = VirtualModulesFactory::create();
        } else {
            systemModule = CanModulesFactory::create();
        }

        //sleep for 1s
        std::this_thread::sleep_for(std::chrono::seconds(1));

        Script script = Parser::parse(argv[1]);

        std::cout << "parsing ok" << std::endl;

        for (const auto & block : script.blockNames()) {
            std::cout << "Block: " << block << std::endl;
            //std::cout << "  Line: " << block.second->line.lineNumber() << " " << block.second->line.content << std::endl;
            //for (auto & nestedBlock : block.second->nestedBlocks) {
            //    std::cout << "  Nested Block: " << nestedBlock->line.lineNumber << " " << nestedBlock->line.content << std::endl;
            //}
        }

        ParseContext::Ptr pctx = std::make_shared<ParseContext>(script);

        auto cmd = Interpreter::build(script.getNamedBlock("main"), pctx);    

        Stack::Ptr stack = std::make_shared<Stack>([&](const Stack::Data & data) {
            if (debugMode) {
                std::cout << "Stack: " << std::endl;
                for (const auto & i : data) {
                    try {
                        std::cout << i << " " << pctx->script.lines().at(i) << std::endl;
                    } catch (std::exception & e) {
                        std::cout << i << " " << e.what() << std::endl;
                    }
                }
                std::cout << std::endl;
            }
        });

        RunContext::Ptr rctx = std::make_shared<RunContext>(stack, systemModule);

        std::cout << "starting ... " << std::endl;
        cmd->run(rctx);
        std::cout << "done. " << std::endl;

    } catch (std::exception & e) {
        std::cerr << "EXCEPTION: " << e.what() << std::endl;
    }
    
    return 0;
}
