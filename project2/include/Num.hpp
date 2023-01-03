#pragma once
#include "Constants.hpp"

#include <array>
#include <ostream>
#include <tuple>

class Num {
private:
    Number integerPart;
    Number floatPart;
    Number ePart;
    int sign;
    int eSign;

public:
    explicit Num() = default;
    explicit Num(const std::array<Number, 3>& rhs, int sign, int eSign);
    Num(const Num&) = default;
    Num(Num&&) noexcept;
    Num& operator=(const Num& rhs) = default;
    Num& operator=(Num&& rhs) noexcept;

    std::tuple<Number*, Number*, Number*, int*, int*> getAll();

    void showContent();

    friend std::ostream& operator<<(std::ostream& o, const Num& rhs);
};
