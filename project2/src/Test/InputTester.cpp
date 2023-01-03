#include "Constants.hpp"
#include "Input.hpp"
#include "TestHelper.hpp"
#include "Testers.hpp"
#include <iostream>
#include <ostream>
#include <random>
#include <string>

GENERATE_TESTERS_DEFINITION(InputTester)

void InputTester::doTest() const {
    std::random_device r;
    std::default_random_engine e(r());
    std::uniform_int_distribution<int> rNum(0, 9), rOperator(0, 6), rIf(0, 1), rSpace(0, 10);
    Input inputerForTest;
    try {
        testLooper([&]() {
            inputerForTest.inputString.clear();
            int space = rSpace(e);
            for(int i = 0; i != 100; ++i) {
                inputerForTest.inputString += std::to_string(rNum(e));
                if(rIf(e)) {
                    auto t = rOperator(e);
                    if(5 == t || 6 == t)
                        inputerForTest.inputString += "()";
                    else
                        inputerForTest.inputString += ValidOperators[t];
                }
            }
            inputerForTest.inputString += std::to_string(rNum(e));
            inputerForTest.produce();
        });
    } catch(InputException& e) {
        std::cout << "Raw String: " + inputerForTest.inputString << std::endl << "error at \'" + e.rawString + "\' ";
        throw TestException(e.reason);
    }
}
