#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <stack>
#include <memory>

using namespace std;

// Dopředná deklarace funkce
void executeBlock(const string& blockName);

// Rozhraní pro příkazy
class ICommand {
public:
    virtual void execute(stack<pair<int, size_t>>& loopStack, vector<unique_ptr<ICommand>>& commands, size_t& i) = 0;
    virtual ~ICommand() = default;
};

// Konkrétní příkazy
class LightCommand : public ICommand {
    string state;
public:
    LightCommand(const string& s) : state(s) {}
    void execute(stack<pair<int, size_t>>& loopStack, vector<unique_ptr<ICommand>>& commands, size_t& i) override {
        cout << "Executing: light " << state << endl;
    }
};

class WaitCommand : public ICommand {
    int time;
public:
    WaitCommand(int t) : time(t) {}
    void execute(stack<pair<int, size_t>>& loopStack, vector<unique_ptr<ICommand>>& commands, size_t& i) override {
        cout << "Executing: wait " << time << " ms" << endl;
    }
};

class MixCommand : public ICommand {
public:
    void execute(stack<pair<int, size_t>>& loopStack, vector<unique_ptr<ICommand>>& commands, size_t& i) override {
        cout << "Executing: mix" << endl;
    }
};

class MeasureCommand : public ICommand {
public:
    void execute(stack<pair<int, size_t>>& loopStack, vector<unique_ptr<ICommand>>& commands, size_t& i) override {
        cout << "Executing: measure" << endl;
    }
};

// Příkaz pro cyklus for
class ForCommand : public ICommand {
    int count;
    size_t loopStart;
public:
    ForCommand(int c, size_t start) : count(c), loopStart(start) {}
    void execute(stack<pair<int, size_t>>& loopStack, vector<unique_ptr<ICommand>>& commands, size_t& i) override {
        if (count > 0) {
            loopStack.push({count, i});
        }
    }
};

// Příkaz pro volání bloku
class CallBlockCommand : public ICommand {
    string blockName;
public:
    CallBlockCommand(const string& name) : blockName(name) {}
    void execute(stack<pair<int, size_t>>& loopStack, vector<unique_ptr<ICommand>>& commands, size_t& i) override {
        cout << "Executing block: " << blockName << endl;
        executeBlock(blockName);
    }
};

// Továrna na příkazy
class CommandFactory {
public:
    static unique_ptr<ICommand> createCommand(const string& cmd, istringstream& ss, size_t commandIndex) {
        if (cmd == "light") {
            string state;
            ss >> state;
            return make_unique<LightCommand>(state);
        } else if (cmd == "wait") {
            int time;
            ss >> time;
            return make_unique<WaitCommand>(time);
        } else if (cmd == "mix") {
            return make_unique<MixCommand>();
        } else if (cmd == "measure") {
            return make_unique<MeasureCommand>();
        } else if (cmd == "for") {
            int count;
            ss >> count;
            return make_unique<ForCommand>(count, commandIndex);
        } else {
            return make_unique<CallBlockCommand>(cmd);
        }
    }
};

// Definice bloku skriptu
struct Block {
    vector<unique_ptr<ICommand>> commands;
};

unordered_map<string, Block> scriptBlocks; // Mapa bloků skriptu

// Načtení skriptu a vytvoření bloků
void parseScript(const string& filename) {
    ifstream file(filename);
    if (!file) {
        cerr << "Error opening file!\n";
        return;
    }
    
    string line;
    string currentBlock;
    int expectedIndent = -1;
    int lineNumber = 0;
    vector<string> scriptLines;
    
    while (getline(file, line)) {
        scriptLines.push_back(line);
    }
    file.close();
    
    // První průchod - identifikace bloků
    for (const auto& line : scriptLines) {
        lineNumber++;
        if (line.empty()) continue;
        
        int indent = 0;
        while (indent < line.size() && line[indent] == ' ') indent++;
        
        if (line.back() == ':') { // Definice bloku
            currentBlock = line.substr(indent, line.size() - indent - 1);
            scriptBlocks[currentBlock] = Block();
            expectedIndent = indent + 4;
        }
    }
    
    // Druhý průchod - zpracování příkazů
    lineNumber = 0;
    currentBlock = "";
    for (const auto& line : scriptLines) {
        lineNumber++;
        if (line.empty()) continue;
        
        int indent = 0;
        while (indent < line.size() && line[indent] == ' ') indent++;
        
        if (line.back() == ':') {
            currentBlock = line.substr(indent, line.size() - indent - 1);
        } else if (!currentBlock.empty()) {
            istringstream ss(line.substr(indent));
            string cmd;
            ss >> cmd;
            
            auto command = CommandFactory::createCommand(cmd, ss, scriptBlocks[currentBlock].commands.size());
            if (!command) {
                cerr << "Syntax error on line " << lineNumber << ": Unknown command '" << cmd << "'" << endl;
                return;
            }
            scriptBlocks[currentBlock].commands.push_back(move(command));
        } else {
            cerr << "Syntax error on line " << lineNumber << ": Command outside of any block." << endl;
            return;
        }
    }
}

// Funkce pro vykonání příkazů
void executeBlock(const string& blockName) {
    if (scriptBlocks.find(blockName) == scriptBlocks.end()) {
        cerr << "Block " << blockName << " not found!\n";
        return;
    }
    
    Block& block = scriptBlocks[blockName];
    stack<pair<int, size_t>> loopStack;
    size_t i = 0;
    while (i < block.commands.size()) {
        block.commands[i]->execute(loopStack, block.commands, i);
        
        if (!loopStack.empty()) {
            auto& top = loopStack.top();
            if (--top.first > 0) {
                i = top.second;
            } else {
                loopStack.pop();
                ++i;
            }
        } else {
            ++i;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <script_file>\n";
        return 1;
    }
    
    parseScript(argv[1]);
    executeBlock("start");
    
    return 0;
}
