#include "OperatorHelper.hpp"
#include "Operators.hpp"

#include <iostream>
#include <utility>
#include <variant>

OperatorBase::OperatorBase(const OneSide& lhs, const OneSide& rhs) : lhs(lhs), rhs(rhs) {}

OperatorBase::OperatorBase(const OneSide& lhs) : lhs(lhs), rhs(OneSide()) {}

OperatorBase::OperatorBase(OperatorBase&& rhs) noexcept {
    std::swap(lhs, rhs.lhs);
    std::swap(this->rhs, rhs.rhs);
}

OperatorBase::~OperatorBase() {
    OperatorHelper::deleteSide(lhs);
    OperatorHelper::deleteSide(rhs);
#ifdef CAL_DEBUG
//    std::endl(std::cout << "call destructor");
#endif
}

Num* OperatorBase::getLeft() const {
    return std::visit(overloaded{ [](OperatorBase* oPtr) { return new Num(std::move(oPtr->doOperation())); },
                                  [](Num* nPtr) { return new Num(*nPtr); } },
                      lhs);
}

std::pair<Num*, Num*> OperatorBase::getBothSides() const {
    constexpr auto getPtr = [](const OneSide& side) {
        return std::visit(overloaded{ [](OperatorBase* oPtr) { return new Num(std::move(oPtr->doOperation())); },
                                      [](Num* nPtr) { return new Num(*nPtr); } },
                          side);
    };

    return { getPtr(lhs), getPtr(rhs) };
}

#ifdef CAL_DEBUG
void OperatorBase::showContent() {
    constexpr auto printSide = [](const OneSide& side) { std::visit(overloaded{ [](auto ptr) { ptr->showContent(); } }, side); };
    printSide(lhs);
    printSide(rhs);
}
#endif
