#include "OperatorHelper.hpp"
#include "Operators.hpp"

#include <exception>

GENERATE_OPERATOR_DEFINITION(Plus)

Num Plus::doOperation() {
    auto tmp = getBothSides();
    try {
        return Num(std::move(OperatorHelper::numPlus(tmp)));
    } catch(std::exception& e) {
        delete tmp.first;
        delete tmp.second;
        throw e;
    }
}
