#include <iostream>
#include <SMBR/Interpreter.hpp>
#include <Poco/String.h>
#include <fstream>
#include <sstream>

int main(int argc, char *argv[]) {
    try {

        std::string templateMd = "doc/template.md";
        if (argc > 1) {
            templateMd = argv[1];
        }

        // load template into string

        std::ifstream t(templateMd.c_str());
        std::stringstream buffer;
        buffer << t.rdbuf();
        std::string templateStr = buffer.str();

        std::stringstream runtimeDoc;

        CommandFactory::instance().createDocumentation(runtimeDoc);

        std::string runtimeDocStr = runtimeDoc.str();
        std::string searchStr = "{{__COMMANDS__}}";
        std::string finalStr = Poco::replace(templateStr, searchStr, runtimeDocStr);

        std::cout << finalStr << std::endl;
    } catch (std::exception &e) {
        std::cerr << "EXCEPTION: " << e.what() << std::endl;
    }

    return 0;
}
