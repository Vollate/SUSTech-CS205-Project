#include "Operators.hpp"

GENERATE_OPERATOR_DEFINITION(NumWapper)

Num NumWapper::doOperation() {
    return *getLeft();
}
