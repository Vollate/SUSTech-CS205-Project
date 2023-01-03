#include "test.hpp"

#include <core.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

int main(int argc, char** argv) {
    if(argc == 1)
        throw std::invalid_argument("empty argument");
    int i = 1;

    for(; i < argc; ++i) {
        switch(*argv[i] - '0') {
            case 0:
                checkGenerate();
                continue;
            case 1:
                checkCopy();
                continue;
            case 2:
                checkMemorySafety();
                continue;
            case 3:
                checkSubMatrix();
                continue;
            case 4:
                checkSplitChannel();
                continue;
            case 5:
                checkIterator();
                continue;
            case 6:
                checkOtherFunctions();
                continue;
            default:
                throw std::invalid_argument("invalid arguments");
        }
    }
    return 0;
}
