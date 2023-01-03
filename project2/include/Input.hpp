#pragma once
#include "Constants.hpp"
#include "Operators.hpp"
#include "Testers.hpp"

#include <exception>
#include <istream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

class Input {
    std::string inputString;
    std::vector<int> lowPriorityIndex;
    std::vector<int> highPriorityIndex;
    std::vector<int> sqrtIndex;
    std::vector<int> powIndex;
    std::vector<int> commaIndex;
    std::vector<std::pair<int, int>> braces;
    friend class InputTester;

    void initAll();
    [[maybe_unused]] OperatorBase* genEquation(const std::string& oriString);
    OperatorBase* genOperators(const std::string& oriString);

public:
    explicit Input();

    std::unique_ptr<OperatorBase> produce();

    friend std::istream& operator>>(std::istream& i, Input& trg);
};

class InputException : public std::exception {
public:
    const std::string reason;
    const std::string rawString;

    explicit InputException(std::string reason, std::string rawString);
    explicit InputException(std::string reason);
};
