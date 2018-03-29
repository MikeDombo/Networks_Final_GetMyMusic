#include "HappyPathJSON.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <exception>
#include <vector>

using namespace std;

string currentKey;
JSON *j;
vector<string> keyHierarchy;


class InputParser {
public:
    InputParser(int &argc, char **argv);

    const std::string &getCmdOption(const std::string &option);

    bool cmdOptionExists(const std::string &option);

    bool findCmdHelp();

private:
    std::vector<std::string> tokens;
};


void printOptions() {
    cout << "[1] Access Key\t[2] Get As String\t[3] Get as String With Unicode\t[4] Go Up Level" << endl;
}

void repl();

JSON getLevelDeep() {
    JSON *js = j;
    if (keyHierarchy.size() <= 0) {
        return *j;
    }
    for (auto s : keyHierarchy) {
        js = &js->operator[](s);
    }
    return *js;
}

void printRedBold() {
    cout << "\033[1;31m";
}

void endPrintColor() {
    cout << "\033[0m";
}

void printRedText(const string &s) {
    printRedBold();
    cout << s;
    endPrintColor();
}

void action(const string &s) {
    if (s == "1") {
        cout << "Key: ";
        getline(cin, currentKey);
        try {
            getLevelDeep()[currentKey];
        } catch (exception &e) {
            printRedText("Illegal Key, try again");
            cout << endl;
            repl();
        }

        keyHierarchy.emplace_back(currentKey);
    } else if (s == "2") {
        if (getLevelDeep().jIsString()) {
            cout << getLevelDeep().getString() << endl;
        } else {
            printRedText("Cannot get non-string as a string");
            cout << endl;
        }
    } else if (s == "3") {
        if (getLevelDeep().jIsString()) {
            cout << getLevelDeep().getStringWithUnicode() << endl;
        } else {
            printRedText("Cannot get non-string as a string");
            cout << endl;
        }
    } else if (s == "4") {
        if (keyHierarchy.size() > 0) {
            keyHierarchy.pop_back();
        } else {
            printRedText("You're at the root");
            cout << endl;
        }
    }

    cout << endl;
    repl();
}

void repl() {
    cout << *j << endl;
    cout << getLevelDeep() << endl;

    printOptions();
    string userInput;
    getline(cin, userInput);
    action(userInput);
}

int main(int argc, char **argv) {
    InputParser ip = InputParser(argc, argv);

    if (ip.findCmdHelp()) {
        cout << "Usage: " << argv[0] << " -p [JSON string to parse]" << endl;
        exit(0);
    }
    try {
        if (ip.cmdOptionExists("-p") || ip.cmdOptionExists("--parse")) {
            if (ip.cmdOptionExists("-p")) {
                j = new JSON(ip.getCmdOption("-p"));
                repl();
                return 0;
            }
            if (ip.cmdOptionExists("--parse")) {
                j = new JSON(ip.getCmdOption("--parse"));
                repl();
                return 0;
            }
        } else {
            cout << "Usage: " << argv[0] << " -p [JSON string to parse]" << endl;
            exit(1);
        }
    } catch (exception &e) {
        printRedText("Unable to parse!\n");
    }
}


InputParser::InputParser(int &argc, char **argv) {
    for (int i = 1; i < argc; ++i) {
        this->tokens.push_back(string(argv[i]));
    }
}

const string &InputParser::getCmdOption(const string &option) {
    vector<string>::const_iterator itr;
    itr = find(this->tokens.begin(), this->tokens.end(), option);
    if (itr != this->tokens.end() && ++itr != this->tokens.end()) {
        return *itr;
    }
    static const string empty_string("");
    return empty_string;
}

bool InputParser::cmdOptionExists(const string &option) {
    return find(this->tokens.begin(), this->tokens.end(), option)
           != this->tokens.end();
}

bool InputParser::findCmdHelp() {
    return this->cmdOptionExists("-h") || this->cmdOptionExists("--help")
           || this->cmdOptionExists("/?") || this->cmdOptionExists("-?")
           || this->cmdOptionExists("?");
}
