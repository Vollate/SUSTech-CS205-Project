#include "OperatorHelper.hpp"
#include "Operators.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <exception>

GENERATE_OPERATOR_DEFINITION(Multiply)

using namespace OperatorHelper;

Num Multiply::doOperation() {
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
        if(lWhole.empty() || rWhole.empty()) {
            delete lhsPtr;
            delete rhsPtr;
            return Num(std::array<Number, 3>{ Number(), Number(), Number() }, 0, 0);
        }
        Number resWhole(lWhole.size());
        std::fill(resWhole.begin(), resWhole.end(), 0);
        for(int i = rWhole.size() - 1, n = 0; i != -1; --i, ++n) {
            auto varNumber = lWhole;
            for(int j = 0; j != n; ++j)
                varNumber.insert(varNumber.end(), 0);
            for(int j = varNumber.size() - 1; j != -1; --j)
                varNumber[j] *= rWhole[i];
            for(int j = varNumber.size() - 1; j != -1; --j) {
                int p = j;
                while(varNumber[p] > 9) {
                    if(--p < 0)
                        varNumber.insert(varNumber.begin(), static_cast<int>(varNumber[++p] / 10));
                    else
                        varNumber[p] += static_cast<int>(varNumber[1 + p] / 10);
                    varNumber[1 + p] %= 10;
                }
            }
            while(resWhole.size() < varNumber.size())
                resWhole.insert(resWhole.begin(), 0);
            for(int j = static_cast<int64_t>(varNumber.size()) - 1, k = static_cast<int64_t>(resWhole.size()) - 1; j > -1;
                --j, --k) {
                resWhole[k] += varNumber[j];
                int p = k;
                if(resWhole[p] > 9) {
                    if(--p < 0)
                        resWhole.insert(resWhole.begin(), static_cast<int>(resWhole[++p] / 10));
                    else
                        resWhole[p] += static_cast<int>(resWhole[1 + p] / 10);
                    resWhole[1 + p] %= 10;
                }
            }
        }
        Number resE;
        int resESign = 0;
        if(!(*lEs ^ *rEs)) {
            resE = std::move(plusNumbers(*lE, *rE));
            resESign = *lEs;
        } else if(*lEs == 1) {
            auto [n, s] = minusNumbers(*rE, *lE);
            resE = std::move(n);
            resESign = s;
        } else {
            auto [n, s] = minusNumbers(*lE, *rE);
            resE = std::move(n);
            resESign = s;
        }
        int resSign = *lS ^ *rS ? 1 : 0;
        [[maybe_unused]] auto noname = removeZero(resE, true);
        delete lhsPtr;
        delete rhsPtr;
        return Num(std::array<Number, 3>{ std::move(resWhole), Number(), resE }, resSign, resESign);
    } catch(std::exception& e) {
        delete lhsPtr;
        delete rhsPtr;
        throw e;
    }
}
