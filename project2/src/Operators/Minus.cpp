#include "OperatorHelper.hpp"
#include "Operators.hpp"

#include <exception>

GENERATE_OPERATOR_DEFINITION(Minus)

Num Minus::doOperation() {
    auto tmp = getBothSides();
    try {
        [[maybe_unused]] auto [i, f, e, s, eS] = tmp.second->getAll();
        *s = !*s;
        return Num(std::move(OperatorHelper::numPlus(tmp)));
    } catch(std::exception& e) {
        delete tmp.first;
        delete tmp.second;
        throw e;
    }
}
