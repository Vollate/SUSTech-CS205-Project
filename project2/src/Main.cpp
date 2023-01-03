#include "Constants.hpp"
#include "Input.hpp"
#include "TestHelper.hpp"

#include <algorithm>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    if(argc > 1) {
        TestHelper(argc, argv, 10000);
        return 0;
    }

    char enterCheck;
    Input inputer;
    while(true) {
        try {
            std::cout << ">> ";
            std::cin.get(enterCheck);
            if(enterCheck == '\n')
                continue;
            else {
                std::cin.putback(enterCheck);
            }
            std::cin >> inputer;
            auto opr = inputer.produce();

#ifdef CAL_DEBUG
            opr->showContent();
            std::cout << "----\n";
#endif
            auto res = std::move(opr->doOperation());
            std::endl(std::cout << res);
        } catch(InputException& e) {
            if(e.reason == "exit")
                return 0;
            else if("Equation" == e.reason)
                std::cout << "Equation isn't support\n";
            else if("Unmatched brace" == e.reason)
                std::cout << "Unmatched brace\n";
            else if(e.reason == "Invalid input")
                std::cout << e.reason + " at \"" + e.rawString + "\"" << std::endl;
            else
                std::cout << e.reason;
        }
    }
}
