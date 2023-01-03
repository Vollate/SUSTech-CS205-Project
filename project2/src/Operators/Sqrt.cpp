#include "Input.hpp"
#include "OperatorHelper.hpp"
#include "Operators.hpp"

#include <array>
#include <cmath>
#include <cstdint>
#include <exception>
#include <utility>

GENERATE_OPERATOR_DEFINITION(Sqrt)

using namespace OperatorHelper;
Num Sqrt::doOperation() {
    auto lhsPtr = getLeft();
    try {
        auto [lI, lF, lE, lS, lEs] = lhsPtr->getAll();
        if(*lS)
            throw InputException("Sqrt for a negative number");
        Number lWhole(lI->size() + lF->size());
        std::copy(lI->begin(), lI->end(), lWhole.begin());
        std::copy(lF->begin(), lF->end(), lWhole.begin() + lI->size());
        numberMinusUint(*lE, *lEs, lF->size());
        {
            [[maybe_unused]] auto countFor = removeZero(lWhole, true);
            auto countBack = removeZero(lWhole, false);
            if(countBack != 0)
                numberMinusUint(*lE, *lEs, countBack);
        }
        long double ori = numberToUint(lWhole);
        long double resDouble = std::sqrt(ori);
        uint64_t integerPart = static_cast<uint64_t>(resDouble);
        long double floatPart = resDouble - integerPart;
        Number resF;
        for(int i = 0; i != 100; ++i) {
            floatPart *= 10;
            resF.push_back(static_cast<int>(floatPart));
            floatPart -= resF.back();
            if(floatPart <0.0000000000000000000000000000000000000000000000000000000000001)
                break;

        }
        int resESign = *lEs;
        Number resE = std::move(*lE);
        [[maybe_unused]] auto noname = removeZero(resE, true);

        delete lhsPtr;
        return Num(std::array<Number, 3>{ Number(std::move(uintToNumber(integerPart))),resF , resE }, 0, resESign);
    } catch(std::exception& e) {
        delete lhsPtr;
        throw e;
    }
}
