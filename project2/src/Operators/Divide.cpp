#include "Input.hpp"
#include "OperatorHelper.hpp"
#include "Operators.hpp"

#include <array>
#include <cstdint>
#include <exception>
#include <iostream>
#include <iterator>

using namespace OperatorHelper;

GENERATE_OPERATOR_DEFINITION(Divide)

Num Divide::doOperation() {
    auto [lhsPtr, rhsPtr] = getBothSides();
    try {
        auto [lI, lF, lE, lS, lEs] = lhsPtr->getAll();
        auto [rI, rF, rE, rS, rEs] = rhsPtr->getAll();
        Number lWhole(lI->size() + lF->size()), rWhole(rI->size() + rF->size());
        std::copy(lI->begin(), lI->end(), lWhole.begin());
        std::copy(lF->begin(), lF->end(), lWhole.begin() + lI->size());
        std::copy(rI->begin(), rI->end(), rWhole.begin());
        std::copy(rF->begin(), rF->end(), rWhole.begin() + rI->size());
        numberMinusUint(*lE, *lEs, lF->size());
        numberMinusUint(*rE, *rEs, rF->size());
#ifdef CAL_DEBUG
//        std::copy(lE->begin(), lE->end(), std::ostream_iterator<int>(std::cout));
//        std::endl(std::cout);
//        std::copy(rE->begin(), rE->end(), std::ostream_iterator<int>(std::cout));
//        std::endl(std::cout);
#endif
        {
            [[maybe_unused]] auto countFor = removeZero(lWhole, true);
            auto countBack = removeZero(lWhole, false);
            if(countBack != 0)
                numberMinusUint(*lE, *lEs, countBack);
        }
        {
            [[maybe_unused]] auto countFor = removeZero(rWhole, true);
            auto countBack = removeZero(rWhole, false);
            if(countBack != 0)
                numberMinusUint(*rE, *rEs, countBack);
        }
        if(rWhole.empty())
            throw InputException("Divided by zero");
        if(lWhole.empty()) {
            delete lhsPtr;
            delete rhsPtr;
            return Num(std::array<Number, 3>{ Number(), Number(), Number() }, 0, 0);
        }
        uint64_t lUint = numberToUint(lWhole), rUint = numberToUint(rWhole);
        Number resWhole;
        uint64_t divTmp = lUint / rUint;
        Number resI(std::move(uintToNumber(divTmp)));
        Number resF;
        for(int i = 0; i != 100; ++i) {
            lUint -= divTmp * rUint;
            if(lUint == 0)
                break;
            lUint *= 10;
            divTmp = lUint / rUint;
            resF.push_back(static_cast<int>(divTmp));
        }
        Number resE;
        int resESign = 0;
        if(!(*lEs ^ *rEs) && *lEs == 0) {
            auto [n, s] = minusNumbers(*lE, *rE);
            resE = std::move(n);
            resESign = s;
        } else if(!(*lEs ^ *rEs) && *lEs == 1) {
            auto [n, s] = minusNumbers(*lE, *rE);
            resE = std::move(n);
            resESign = s ? 0 : 1;

        } else if(*lEs == 1) {
            resE = std::move(plusNumbers(*lE, *rE));
            resESign = 1;
        } else {
            resE = std::move(plusNumbers(*lE, *rE));
            resESign = 0;
        }
        int resSign = *lS ^ *rS ? 1 : 0;
        [[maybe_unused]] auto noname = removeZero(resE, true);
        delete lhsPtr;
        delete rhsPtr;
        return Num(std::array<Number, 3>{ resI, resF, resE }, resSign, resESign);
    } catch(std::exception& e) {
        delete lhsPtr;
        delete rhsPtr;
        throw e;
    }
}
