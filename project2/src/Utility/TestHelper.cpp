#include "TestHelper.hpp"
#include "Testers.hpp"

#include "magic_enum.hpp"

#include <Python.h>
#include <iostream>
#include <string>

#define PY_SSIZE_T_CLEAN

TestHelper::TestHelper(int count, char** cmdArguments, int testTimes) {
    std::list<TesterBase*> testers;

    for(int i = 1; i != count; ++i)
        try {
            testers.push_back(
                genTest(magic_enum::enum_cast<TestHelper::TestName>(std::string(cmdArguments[i])).value(), testTimes));
        } catch(std::exception& e) {
            throw e;
        }
    for(const auto& atom : testers)
        try {
            atom->doTest();
            delete atom;
        } catch(TestException& e) {
            delete atom;
            std::cout << e.reason << std::endl;
        }
}

TesterBase* TestHelper::genTest(TestName test, int testTimes) {
#define GENERATE_TESTCASE(__tester_name) \
    res = new __tester_name(testTimes);  \
    return res;

    TesterBase* res;
    switch(test) {
        case InputTest:
            GENERATE_TESTCASE(InputTester)
        case PlusTest:
            GENERATE_TESTCASE(PlusTester)
        case MinusTest:
            GENERATE_TESTCASE(MinusTester)
        case MultiplyTest:
            GENERATE_TESTCASE(MultiplyTester)
        case DivideTest:
            GENERATE_TESTCASE(DivideTester)
        case SqrtTest:
            GENERATE_TESTCASE(SqrtTester)
        case PowTest:
            GENERATE_TESTCASE(PowTester)
        default:
            throw;
    }
}

std::string TestHelper::checkWithPython(const std::string& s, TestName test) {
    return "";  // TODO
}

TesterBase::TesterBase() : testTimes(0) {}

TesterBase::TesterBase(int times) : testTimes(times) {}

TesterBase::~TesterBase() {}

TestException::TestException(std::string reason) : reason(std::move(reason)) {}
