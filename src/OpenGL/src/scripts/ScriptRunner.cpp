#include <iostream>
#include <string>

// Include any necessary headers for your scripts
#include "Script1.h"
#include "Script2.h"

void RunScript1() {
    // Code to run Script1
    Script1 script;
    script.Execute();
}

void RunScript2() {
    // Code to run Script2
    Script2 script;
    script.Execute();
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: ScriptRunner <script_name>" << std::endl;
        return 1;
    }

    std::string scriptName = argv[1];

    if (scriptName == "Script1") {
        RunScript1();
    } else if (scriptName == "Script2") {
        RunScript2();
    } else {
        std::cerr << "Unknown script: " << scriptName << std::endl;
        return 1;
    }

    return 0;
}
