#include "OperatorHelper.hpp"
#include "Num.hpp"

#include <array>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <utility>

bool OperatorHelper::checkNull(const OperatorBase::OneSide& side) {
    return std::visit(overloaded{ [](Num* n) { return true; }, [](OperatorBase* o) { return false; } }, side);
}

Number OperatorHelper::uintToNumber(std::uint64_t n) {
    Number res;
    while(n != 0) {
        res.insert(res.begin(), n % 10);
        n /= 10;
    }
    return res;
}

uint64_t OperatorHelper::numberToUint(const Number& n) {
    uint64_t res = 0;
    for(int i = 0; i != n.size(); ++i) {
        res += std::pow(10, i) * n[n.size() - 1 - i];
    }
    return res;
}

Num OperatorHelper::numPlus(std::pair<Num*, Num*>& data) {
    auto [lI, lF, lE, lS, lEs] = data.first->getAll();
    auto [rI, rF, rE, rS, rEs] = data.second->getAll();
    Number lWhole(lI->size() + lF->size()), rWhole(rI->size() + rF->size());
    std::copy(lI->begin(), lI->end(), lWhole.begin());
    std::copy(lF->begin(), lF->end(), lWhole.begin() + lI->size());
    std::copy(rI->begin(), rI->end(), rWhole.begin());
    std::copy(rF->begin(), rF->end(), rWhole.begin() + rI->size());
    numberMinusUint(*lE, *lEs, lF->size());
    numberMinusUint(*rE, *rEs, rF->size());

    int resSign = 0, resESign = 0;
    Number eDiff;
    if(*lEs == 0 && *rEs == 0) {
        auto [n, s] = minusNumbers(*lE, *rE);
        eDiff = n;
        resESign = s;
    } else if(*lEs == 0) {
        eDiff = plusNumbers(*lE, *rE);
    } else if(*rEs == 0) {
        eDiff = plusNumbers(*lE, *rE);
        resESign = 1;
    } else {
        auto [n, s] = minusNumbers(*lE, *rE);
        eDiff = n;
        resESign = s ? 0 : 1;
    }
    if(resESign == 0) {
        for(uint64_t i = 0; i != numberToUint(eDiff); ++i)
            lWhole.push_back(0);
        *lE = *rE;
    } else {
        for(uint64_t i = 0; i != numberToUint(eDiff); ++i)
            rWhole.push_back(0);
        *rE = *lE;
    }
    alignNumbers(lWhole, rWhole);
    Number resWhole;
    if(!(*lS ^ *rS)) {
        resWhole = std::move(plusNumbers(lWhole, rWhole));
        resSign = *lS;
    } else if(*lS == 1) {
        auto [n, s] = minusNumbers(rWhole, lWhole);
        resWhole = std::move(n);
        resSign = s;
    } else {
        auto [n, s] = minusNumbers(lWhole, rWhole);
        resWhole = std::move(n);
        resSign = s;
    }
    Number resE(std::move(*lE));
    delete data.first;
    delete data.second;
#ifdef CAL_DEBUG
//    std::copy(resWhole.begin(), resWhole.end(), std::ostream_iterator<int>(std::cout));
//    std::endl(std::cout);
//    std::copy(resE.begin(), resE.end(), std::ostream_iterator<int>(std::cout));
//    std::endl(std::cout);
#endif
    [[maybe_unused]] auto countFor = removeZero(resWhole, true);
    auto countBack = removeZero(resWhole, false);
    if(countBack != 0)
        numberMinusUint(resE, resESign, countBack);
    [[maybe_unused]] auto noname = removeZero(resE, true);
    return Num(std::array<Number, 3>{ std::move(resWhole), Number(), resE }, resSign, resESign);
}

void OperatorHelper::numberMinusNumber(Number& trg, int& sign, Number& m) {
    {
        if(sign)
            trg = std::move(plusNumbers(trg, m));
        else {
            auto [n, s] = minusNumbers(trg, m);
            trg = n;
            sign = s;
        }
    }
}
void OperatorHelper::numberMinusUint(Number& trg, int& sign, uint64_t n) {
    {
        auto tmpNumber = uintToNumber(n);
        if(sign)
            trg = std::move(plusNumbers(trg, tmpNumber));
        else {
            auto [n, s] = minusNumbers(trg, tmpNumber);
            trg = n;
            sign = s;
        }
    }
}

Number OperatorHelper::plusNumbers(Number& lhs, Number& rhs, bool forward) {
    alignNumbers(lhs, rhs, forward);
    Number res(lhs);
    if(lhs.empty())
        return res;
    for(uint64_t i = 0; i != res.size(); ++i)
        res[i] += rhs[i];
    for(uint64_t i = res.size() - 1; i != 0; --i)
        if(res[i] > 9) {
            res[i - 1] += res[i] / 10;
            res[i] = res[i] % 10;
        }
    if(res[0] > 9) {
        res.insert(res.begin(), res[0] / 10);
        res[1] = res[1] % 10;
    }
    return res;
}
void OperatorHelper::deleteSide(OperatorBase::OneSide& side) {
    std::visit(overloaded{ [](auto ptr) { delete ptr; } }, side);
}

std::pair<Number, int> OperatorHelper::minusNumbers(Number& lhs, Number& rhs, bool forward) {
    alignNumbers(lhs, rhs, forward);
    int sign = 0;
    for(int i = 0; i != lhs.size(); ++i) {
        if(lhs[i] < rhs[i]) {
            sign = 1;
            break;
        } else if(lhs[i] > rhs[i])
            break;
    }
    Number res;
    if(sign) {
        res = rhs;
        for(uint64_t i = 0; i != res.size(); ++i)
            res[i] -= lhs[i];
    } else {
        res = lhs;
        for(uint64_t i = 0; i != res.size(); ++i)
            res[i] -= rhs[i];
    }
    if(lhs.empty())
        return { res, 0 };
    for(uint64_t i = res.size() - 1; i != 0; --i)
        if(res[i] < 0) {
            res[i - 1] -= 1;
            res[i] += 10;
        }
    return { res, sign };
}

void OperatorHelper::alignNumbers(Number& lhs, Number& rhs, bool forward) {
    std::int64_t diff = static_cast<std::int64_t>(lhs.size()) - static_cast<std::int64_t>(rhs.size());
    if(diff == 0)
        return;
    else if(diff > 0) {
        if(forward)
            for(int64_t i = 0; i != diff; ++i)
                rhs.insert(rhs.begin(), 0);
        else
            for(int64_t i = 0; i != diff; ++i)
                rhs.push_back(0);
    } else {
        if(forward)
            for(int64_t i = diff; i != 0; ++i)
                lhs.insert(lhs.begin(), 0);
        else
            for(int64_t i = diff; i != 0; ++i)
                lhs.push_back(0);
    }
}

uint64_t OperatorHelper::removeZero(Number& trg, bool forward) {
    if(trg.empty())
        return 0;
    uint64_t count = 0;
    if(forward) {
        for(uint64_t i = 0; i != trg.size(); ++i)
            if(trg[i] != 0) {
                if(i != 0) {
                    trg.erase(trg.begin(), trg.begin() + i);
                    count = i;
                }
                return count;
            }
        trg.clear();
    } else {
        for(uint64_t i = trg.size() - 1; i != -1; --i)
            if(trg[i] != 0) {
                if(i != trg.size() - 1) {
                    trg.erase(trg.begin() + i, trg.end());
                    count = trg.size() - i;
                }
                return count;
            }
        trg.clear();
    }
}
