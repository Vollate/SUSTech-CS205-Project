#pragma once
#include "Constants.hpp"
#include "Num.hpp"
#include "Operators.hpp"

#include <cstdint>
#include <functional>
#include <utility>

namespace OperatorHelper {

    bool checkNull(const OperatorBase::OneSide& side);

    Number uintToNumber(uint64_t n);

    uint64_t numberToUint(const Number& n);

    void deleteSide(OperatorBase::OneSide& side);

    Num numPlus(std::pair<Num*, Num*>& data);

    Number plusNumbers(Number& lhs, Number& rhs, bool forward = true);

    std::pair<Number, int> minusNumbers(Number& lhs, Number& rhs, bool forward = true);

    void numberMinusUint(Number& trg, int& sign, uint64_t n);

    void numberMinusNumber(Number& trg, int& sign, Number& m);

    void alignNumbers(Number& lhs, Number& rhs, bool forward = true);

    uint64_t removeZero(Number& trg, bool forward);

}  // namespace OperatorHelper
