#pragma once
#include <functional>
#include <random>

#define GENERATE_TESTERS_DECLARATION(__sub_class_name)            \
    class __sub_class_name : public TesterBase {                  \
    public:                                                       \
        explicit __sub_class_name(int times);                     \
        __sub_class_name& operator=(const __sub_class_name& rhs); \
        virtual ~__sub_class_name() = default;                    \
        virtual void doTest() const;                              \
    };

#define GENERATE_TESTERS_DEFINITION(__sub_class_name)                            \
    __sub_class_name::__sub_class_name(int times) : TesterBase(times) {}         \
    __sub_class_name& __sub_class_name::operator=(const __sub_class_name& rhs) { \
        testTimes = rhs.testTimes;                                               \
        return *this;                                                            \
    }

class TesterBase {
protected:
    int testTimes;
    const std::function<void(const std::function<void(void)>&)> testLooper = [this](const std::function<void(void)>& func) {
        for(int i = 0; i != testTimes; ++i)
            func();
    };

public:
    explicit TesterBase();
    explicit TesterBase(int times);
    virtual void doTest() const = 0;
    virtual ~TesterBase();
};

GENERATE_TESTERS_DECLARATION(InputTester)
GENERATE_TESTERS_DECLARATION(PlusTester)
GENERATE_TESTERS_DECLARATION(MinusTester)
GENERATE_TESTERS_DECLARATION(MultiplyTester)
GENERATE_TESTERS_DECLARATION(DivideTester)
GENERATE_TESTERS_DECLARATION(SqrtTester)
GENERATE_TESTERS_DECLARATION(PowTester)
