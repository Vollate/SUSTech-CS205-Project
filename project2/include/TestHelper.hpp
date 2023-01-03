#pragma once
#include "Testers.hpp"

#include <functional>
#include <list>
#include <memory>
#include <string>

class TestHelper {
    enum TestName { InputTest, PlusTest, MinusTest, MultiplyTest, DivideTest, SqrtTest, PowTest };
    TesterBase* genTest(TestName singleTest, int testTimes);

public:
    explicit TestHelper(int count, char** cmdArguments, int testTimes);
    static std::string checkWithPython(const std::string& s, TestName test);
};

class TestException : public std::exception {
public:
    std::string reason;
    explicit TestException(std::string reason);
};
