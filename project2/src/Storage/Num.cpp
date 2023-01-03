#include "Num.hpp"

#include <algorithm>
#include <array>
#include <iostream>
#include <iterator>
#include <ostream>
#include <tuple>
#include <vector>

Num::Num(const std::array<Number, 3>& rhs, int sign, int eSign)
    : integerPart(rhs[0]), floatPart(rhs[1]), ePart(rhs[2]), sign(sign), eSign(eSign) {}

Num::Num(Num&& rhs) noexcept
    : integerPart(std::move(rhs.integerPart)), floatPart(std::move(rhs.floatPart)), ePart(std::move(rhs.ePart)), sign(rhs.sign),
      eSign(rhs.eSign) {}

Num& Num::operator=(Num&& rhs) noexcept {
    std::swap(integerPart, rhs.integerPart);
    std::swap(floatPart, rhs.floatPart);
    std::swap(ePart, rhs.ePart);
    std::swap(sign, rhs.sign);
    std::swap(eSign, rhs.eSign);
    return *this;
}

std::tuple<Number*, Number*, Number*, int*, int*> Num::getAll() {
    return { &integerPart, &floatPart, &ePart, &sign, &eSign };
}

void Num::showContent() {
    auto [i, f, e, s, es] = getAll();
    if(*s == 1)
        std::cout << "-";
    std::copy(i->begin(), i->end(), std::ostream_iterator<int>(std::cout));
    std::cout << ".";
    std::copy(f->begin(), f->end(), std::ostream_iterator<int>(std::cout));
    std::cout << "E";
    if(*es == 1)
        std::cout << "-";
    std::copy(e->begin(), e->end(), std::ostream_iterator<int>(std::cout));
    std::cout << "\n";
}

std::ostream& operator<<(std::ostream& o, const Num& rhs) {
    if(rhs.integerPart.empty() && rhs.floatPart.empty()) {
        o << 0;
        return o;
    }
    if(rhs.sign == 1)
        o << "-";
    if(rhs.integerPart.empty())
        o << 0;
    else
        for(int integer : rhs.integerPart)
            o << integer;
    if(!rhs.floatPart.empty()) {
        o << ".";
        for(int floa : rhs.floatPart)
            o << floa;
    }
    if(!rhs.ePart.empty()) {
        o << "E";
        if(rhs.eSign == 1)
            o << "-";
        for(int e : rhs.ePart)
            o << e;
    }
    return o;
}
