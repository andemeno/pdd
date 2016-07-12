#include "programconfig.h"
#include <fstream>

using namespace pdd;

std::string Config::largePromptText() const {
    if(usePromptFromFile) {

        std::ifstream file("../pdd_resources/prompt.txt");
        if(!file.is_open())
            return "";

        std::string lines;
        while(!file.eof()) {
            std::string line;
            std::getline(file, line);
            lines.append(line);
            lines.append("\n");
        }

        return lines;
    } else {
        if(protocolVersion == 0) return largePromptText0;
        if(protocolVersion == 1) return largePromptText1;
    }
    return "";
}
