#pragma once
#include <array>
#include <string>
#include <vector>

typedef std::vector<int> Number;
const std::array<std::string, 3> NotException{ "empty_input", "help", "Equation" };
constexpr std::array<char, 11> ValidOperators{ '+', '-', '*', '/', '.', ',', '(', ')', '=', 'e', 'E' };
const std::string MANUAL(R"(Command list:
 exit    terminate the calculator
)");
