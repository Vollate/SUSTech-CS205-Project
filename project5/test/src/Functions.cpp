#include "test.hpp"

#include <cfloat>
#include <core.hpp>
#include <exception>
#include <iostream>
#include <random>
#include <stdexcept>

#define MAX_EXPAND 514
#define separate_line std::cout << "===================================\n"
std::random_device r;
std::default_random_engine e(r());
std::uniform_int_distribution<size_t> ran_sizet(1, 1234);
std::uniform_real_distribution<double> ran_double(DBL_MIN, DBL_MAX);

template <typename _T, size_t _C>
struct ExpandType {
    _T data;
    void generate() {
        {
            rmat::Mat_<ExpandType<_T, _C>> tmp(11451, 19198);
            std::cout << "current level: " << _C << "\t" << tmp.channelSize() << "\n";
        }
        ExpandType<_T, _C + 1>().generate();
    }
};
template <typename _T>
struct ExpandType<_T, MAX_EXPAND + 1> {
    void generate() {}
};
void checkGenerate() {
    // check correct case
    using namespace rmat;
    bool useless;
    {
        Mat_<int> tmp;
        useless = tmp.isContinuous();
    }
    {
        Mat_<char> tmp(ran_sizet(e), ran_sizet(e), { '1', '\0', '#' });
        useless = tmp.isContinuous();
    }
    if(useless) {
        ExpandType<int, 1> tmp;
        tmp.generate();
    }
    try {
        Mat_<double> tmp(4, 6, 0);
        throw std::logic_error("exception doesn't throw");
    } catch(std::exception& e) {
        std::cout << "catch successfully: " << e.what() << std::endl;
    }
    separate_line;
}

void checkCopy() {
    rmat::Mat_<long double> src(11454, 19198);
    auto a = src.copy();
    rmat::Mat_<long double> b;
    src.copy(b);
}

void checkMemorySafety() {
    for(int i = 0; i <= 514; ++i) {
        rmat::Mat_<size_t> tmp(1e4, 1e3, 5);
        auto ptr = tmp.ptr();
        for(size_t j = 0; j < 5e7; ++j)
            *(ptr++) = j;
        std::endl(std::cout << "current allocate time: " << i);
    }
    separate_line;
}

void checkSubMatrix() {
    rmat::Mat_<float> testA(5000, 4000, { 1, 2, 3, 4 });
    auto a_1 = testA.getSubMat(1, 2, 500, 600, 2, false);
    testA.splitChannel(2);
    auto a_3 = testA.getSubMat(3, 5, 500, 600, false);
    if(a_1 != a_3)
        throw std::logic_error("submat error");
}

void checkSplitChannel() {
    std::vector<char> tmp{ '1', '*' };
    rmat::Mat_<char> test{ 34517, 3945, tmp };
    test.splitChannel(1);
    std::for_each(test.begin(), test.end(), [](char c) {
        if(c != '1')
            throw std::logic_error("not equal");
    });
}

void checkIterator() {
    rmat::Mat_<double> test(2000, 2000, 3);
    auto ptr = test.ptr();
    for(size_t i = 0; i < test.rowSize()*test.colSize()*test.channelSize(); ++i)
        *ptr++ = ran_double(e);
    auto channel = test.channelSize();
    std::for_each(test.begin(), test.end(), [channel](auto& tmp) {
        auto ptr = &tmp;
        for(int i = 0; i < channel; ++i)
            *ptr++ = i;
    });
    std::cout << test;
}

void checkOtherFunctions() {
    rmat::Mat_<long> testA(1000, 2000, { 8, 391, 2 });
    rmat::Mat_<long> testB;
    testA.copy(testB, true);
    testB.clear();
    if(!testB.empty())
        throw std::logic_error("wrong");
    testB.realloc(4, 5, 2);
}
