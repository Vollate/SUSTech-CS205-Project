# Project 2

    12111224
    贾禹帆
>注意：为避免运行时出现调试信息，请勿以Debug选项生成cmake缓存

<!-- vim-markdown-toc GFM -->

- [Framework](#framework)
    - [class `Input`](#class-input)
    - [class `Num`](#class-num)
    - [class `OpertorBase`](#class-opertorbase)
    - [class `TestBase`](#class-testbase)
    - [class `TestHelper`](#class-testhelper)
    - [function `main`](#function-main)
    - [Files Tree View](#files-tree-view)
- [各模块实现思路](#各模块实现思路)
    - [输入处理](#输入处理)
    - [高精度加减法](#高精度加减法)
    - [高精度乘法](#高精度乘法)
    - [高精度除法](#高精度除法)
    - [开方](#开方)
    - [测试模块](#测试模块)
    - [主函数](#主函数)
- [部分代码详解](#部分代码详解)
    - [Main.cpp](#maincpp)
    - [Operators.hpp](#operatorshpp)
    - [OperatorHelper.hpp](#operatorhelperhpp)
    - [Input.cpp](#inputcpp)
- [结果展示](#结果展示)
    - [功能](#功能)
    - [非法输入检测](#非法输入检测)
- [总结&心得](#总结心得)

<!-- vim-markdown-toc -->
本项目使用的开源库如下:

- [magic_enum](https://github.com/Neargye/magic_enum)

## Framework

本次project主要需求:

- 实现复杂字符串输入处理
- 实现高精度加减乘除开平方和次方

分析:由于操作符具有相似的特性(一个操作符操作左右两边的数据),因此可以使用多态来重载各操作符的行为到一个基类上，考虑到优先级，需要链式创建操作符，所以使用了variant

框架构建思路:
一个计算器需要一套和用户交互的接口来接收数据输入和输出数据,需要一个处理输入的模块来检验输入合法性和转换成可计算的数据。需要一个用来存储数据的数据结构和一个用来处理数据的模块。为了便于调试，还需要测试模块，框架设计如下：

### class `Input`

接收输入并进行字符串处理

```c++
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
```

### class `Num`

以高精度存储数据

```c++
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
```

### class `OpertorBase`

定义基本操作符，提供`doOperation()`操作接口,同时利用类管理内存(RAII)

```c++
class OperatorBase {
public:
    typedef std::variant<Num*, OperatorBase*> OneSide;

protected:
    OneSide lhs;
    OneSide rhs;
    [[nodiscard]] std::pair<Num*, Num*> getBothSides() const;

public:
    explicit OperatorBase(const OneSide& lhs, const OneSide& rhs);
    explicit OperatorBase(const OneSide& lhs);
    OperatorBase(OperatorBase&& rhs) noexcept;
    OperatorBase& operator=(const OperatorBase& rhs) = default;
    virtual ~OperatorBase();

    void showContent();
    virtual Num doOperation() = 0;
};
```

### class `TestBase`

提供基本的测试操作接口 `doTest()`

```c++
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
```

### class `TestHelper`

用于初始化各个`tester`并执行测试，并和python校验答案

```c++
class TestHelper {
    enum TestName { InputTest, PlusTest, MinusTest, MultiplyTest, DivideTest, SqrtTest, PowTest };
    TesterBase* genTest(TestName singleTest, int testTimes);

public:
    explicit TestHelper(int count, char** cmdArguments, int testTimes);
    static std::string checkWithPython(const std::string& s, TestName test);
};
```

### function `main`

实现输入输出,测试和异常捕获

```c++
int main(int argc, char** argv) {
    if(argc > 1) {
        TestHelper(argc, argv, 10000);
        return 0;
    }

    char enterCheck;
    Input inputer;
    while(true) {
        try {
            std::cout << ">> ";
            std::cin.get(enterCheck);
            if(enterCheck == '\n')
                continue;
            else {
                std::cin.putback(enterCheck);
            }
            std::cin >> inputer;
            auto opr = inputer.produce();

#ifdef CAL_DEBUG
//            opr->showContent();
//            std::cout << "----\n";
#endif
            auto res = std::move(opr->doOperation());
            std::endl(std::cout << res);
        } catch(InputException& e) {
            if(e.reason == "exit")
                return 0;
            else if("Equation" == e.reason)
                std::cout << "Equation isn't support\n";
            else if("Unmatched brace" == e.reason)
                std::cout << "Unmatched brace\n";
            else if(e.reason == "Invalid input")
                std::cout << e.reason + " at \"" + e.rawString + "\"" << std::endl;
            else
                std::cout << e.reason;
        }
    }
}
```

- 除了以上基本类外，还有对应`*Helper`文件来分离不需要使用类内成员变量的函数，以降低单个文件长度并提高代码复用率(函数封装在对应名称空间内)

---
**Program flowchart**
<!--
```flowchart
st=>start: main
e=>end: exit
doTest?=>condition: argc>1?
tester=>subroutine: Testers(do test)
testResult=>inputoutput: output test result
inputer=>subroutine: Inputer(get user input)
exit?=>condition: exit?
operator=>subroutine: Operators(do calculation)
valid?=>condition: valid input?
operatorResult=>inputoutput: output the final result
st->doTest?
doTest?(yes,right)->tester->testResult->e
doTest?(no,left)->inputer->exit?
exit?(yes,right)->e
exit?(no,left)->valid?
valid?(no,right)->inputer
valid?(yes,left)->operator->operatorResult->inputer
```
-->
![flowchart-img](https://github.com/uint44t/SUSTC_CS205-CPP/blob/main/project2/img/flowchart.png?raw=true)

### Files Tree View

```text
project2
    ├─img    //images
    ├─include    //headers
    ├─library    //outer library
    └─src    //source files
        ├─Operators
        ├─Storage
        ├─Test
        └─Utility
```

---

## 各模块实现思路

### 输入处理

这次的字符串处理相较于上一次增加了不少难度，首先运算符数量增加，并且增加了`pow,sqrt`这两个字符串运算符和`(,)`
让优先级判断和非法输入检查变得更加麻烦(~~之前光乘法就不用判断优先级~~)思路如下:

- 遍历字符串，对在$[0,47]\cap[58,127]$范围内的字符进行如下判断:
    1. 是否属于`{ '+', '-', '*', '/', '.', ',', '(', ')', '=', 'e', 'E' }`
    2. 若不属于，是否是 `"sqrt(","pow("`, 不是则非法
- 遍历字符串中`'='`个数，1个为方程，0个为算式，其余非法
- 处理算式:
  - 对于各个操作符，有以下限制条件
    - `*,/`左右两边必须为数字
    - `+,-`左边可以为空，但右边必须为数字(可以当作一元或二元操作符)
    - `sqrt,pow`后必须立即跟上括号
    - 对于一个数字，只能有一个小数点`.`和一个`e/E`
    - `.`前后至少有一个数字
    - `e/E`前后必须有数字
  - 整理所有情况:
    - `+,-`前合法的字符有`+,-,*,/,E,e,(,)`，后合法的字符有`+,-,s,p,(`  
          注意到这时会有`+++`这种情况，所以应该再向前一个字符进行检查,此时:
            1. 若前一个字符是`(`，则`+,-，*，/，(`合法
            2. 此外，只有`)`合法
    - `*,/`前合法的字符有`)`,后合法的字符有`+,-,s,p,(`
    - `e,E`前合法的字符有`null(除了数字以外全部非法)`，后合法的字符有`+,-`
    - `sqrt,pow`前合法的字符有`+,-,*,/,(`(不包含数字),后合法的字符有`(`
    - `.`前后都必须为数字
    - **此外还要记得和边界问题一起处理**

  - 遍历字符串，并将下标放至对应数组，同时判断是否满足限制条件
    - `'+','-'`(非一元操作符) lowPriorityIndex
    - `'*','/'` highPriorityIndex
    - `'(',')'` braces(若不成对则`throw`)
    - `"pow","sqrt"` powIndex, sqrtIndex
  - 优先级(高到低)
        1. 一元`+,-`(eg: `-1*10`,`sqrt(+10)`)
        2. `()`(包括`sqrt,pow`运算符的括号)
        3. `*,/`
        4. 二元`+,-`
  - 优先级判断思路:
    - 括号内优先级比括号外高(`sqrt,pow`的优先级也就是他们括号的优先级),因此先遍历所有括号的下标，确定各个括号的优先级顺序
    - 对括号内的运算符进行排序，最后得到表示优先级顺序的数组
  - 根据优先级顺序的数组生成操作(`operator`),从优先级大的区间开始生成，操作过程中需要校验各个数字的合法性

### 高精度加减法

先将两边化为同一位数(科学计数法e的指数相同)，然后根据正负号进行加或减运算

- 先将整数部分和小数部分拼接起来，并根据偏移值修改对应E的数值(注意考虑E的符号)
- 将左右两数对齐(短数向前补零)，同时调整各自的E值
- 进行加减法运算

### 高精度乘法

- 首先判断乘数是否为零
- 按位分别相乘再相加
- 对两数符号进行异或运算，相同结果为正，否则为负
- 指数部分相加

### 高精度除法

- 首先判断被除数，除数是否为0(除数为零为非法情况)
- 将各数整数与小数部分合并并调整各自指数大小
- 进行除法直到达到精度(用于过滤浮点数误差)或达到要求位数(默认100位)

### 开方

- 转化为浮点数后开方

### 测试模块

- 使用magic_enum进行编译期反射，将字符串映射成对应的枚举
- 在cmake中添加ctest运行程序并传入命令行参数执行对应测试
- 由于python自带高精度，使用cpython库的API调用python执行结果校验

>由于时间原因，tester仅搭好框架，并未来得及实现

### 主函数

- 持续接受输入并且计算，直到接收到"exit"指令
- 使用`try{  }catch{  }`捕获异常(非法输入等)然后继续程序的运行

>定义`InputException`类(继承自std::Exception)用于表示输入异常的类型

## 部分代码详解

### Main.cpp

```c++
#include "Constants.hpp"
#include "Input.hpp"
#include "TestHelper.hpp"

#include <algorithm>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    if(argc > 1) {
        TestHelper(argc, argv, 10000);
        return 0;
    }

    char enterCheck;
    Input inputer;
    while(true) {
        try {
            std::cout << ">> ";
            std::cin.get(enterCheck);
            if(enterCheck == '\n')
                continue;
            else {
                std::cin.putback(enterCheck);
            }
            std::cin >> inputer;
            auto opr = inputer.produce();

#ifdef CAL_DEBUG
            opr->showContent();
            std::cout << "----\n";
#endif
            auto res = std::move(opr->doOperation());
            std::endl(std::cout << res);
        } catch(InputException& e) {
            if(e.reason == "exit")
                return 0;
            else if("Equation" == e.reason)
                std::cout << "Equation isn't support\n";
            else if("Unmatched brace" == e.reason)
                std::cout << "Unmatched brace\n";
            else if(e.reason == "Invalid input")
                std::cout << e.reason + " at \"" + e.rawString + "\"" << std::endl;
            else
                std::cout << e.reason;
        }
    }
}

```

### Operators.hpp

```c++
#pragma once
#include "Num.hpp"

#include <memory>
#include <utility>
#include <variant>
//使用宏进行派生类的声明和定义
#define GENERATE_OPERATOR_DECLARATION(__operator_name)                                                \
    class __operator_name : public OperatorBase {                                                     \
    public:                                                                                           \
        explicit __operator_name(const OperatorBase::OneSide& lhs, const OperatorBase::OneSide& rhs); \
        explicit __operator_name(const OperatorBase::OneSide& lhs);                                   \
        __operator_name& operator=(const __operator_name& rhs);                                       \
        virtual ~__operator_name();                                                                   \
        virtual Num doOperation();                                                                    \
    };

#define GENERATE_OPERATOR_DEFINITION(__operator_name)                                                    \
    __operator_name::__operator_name(const OperatorBase::OneSide& lhs, const OperatorBase::OneSide& rhs) \
        : OperatorBase(lhs, rhs) {}                                                                      \
    __operator_name::__operator_name(const OperatorBase::OneSide& lhs) : OperatorBase(lhs) {}            \
    __operator_name& __operator_name::operator=(const __operator_name& rhs) {                            \
        this->lhs = rhs.lhs;                                                                             \
        this->rhs = rhs.rhs;                                                                             \
        return *this;                                                                                    \
    }                                                                                                    \
    __operator_name::~__operator_name() = default;

class OperatorBase {
public:
    typedef std::variant<Num*, OperatorBase*> OneSide;

protected:
    OneSide lhs;
    OneSide rhs;
    std::pair<Num*, Num*> getBothSides() const;
    Num* getLeft() const;

public:
    explicit OperatorBase(const OneSide& lhs, const OneSide& rhs);
    explicit OperatorBase(const OneSide& lhs);
    OperatorBase(OperatorBase&& rhs) noexcept;
    OperatorBase& operator=(const OperatorBase& rhs) = default;
    virtual ~OperatorBase();

    void showContent();
    virtual Num doOperation() = 0;
};

//优雅地通过重载使用variant
template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};
//此条在c++20不再需要
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

GENERATE_OPERATOR_DECLARATION(Plus)
GENERATE_OPERATOR_DECLARATION(Minus)
GENERATE_OPERATOR_DECLARATION(Multiply)
GENERATE_OPERATOR_DECLARATION(Divide)
GENERATE_OPERATOR_DECLARATION(Sqrt)
GENERATE_OPERATOR_DECLARATION(Pow)
GENERATE_OPERATOR_DECLARATION(NumWapper)

```

### OperatorHelper.hpp

```c++
#pragma once
#include "Constants.hpp"
#include "Num.hpp"
#include "Operators.hpp"

#include <cstdint>
#include <functional>
#include <utility>

namespace OperatorHelper {/*typedef std::vector<int> Number;*/

    bool checkNull(const OperatorBase::OneSide& side);//检查variant是否为空

    Number uintToNumber(uint64_t n);//将uint64_t转为Number

    uint64_t numberToUint(const Number& n);

    void deleteSide(OperatorBase::OneSide& side);//删除varient

    Num numPlus(std::pair<Num*, Num*>& data);//对两个Num类执行相加操作

    Number plusNumbers(Number& lhs, Number& rhs, bool forward = true);//将两个Number按位相加

    std::pair<Number, int> minusNumbers(Number& lhs, Number& rhs, bool forward = true);//将两个Number按位相减

    void numberMinusUint(Number& trg, int& sign, uint64_t n);//vectro<int> - uint64_t

    void numberMinusNumber(Number& trg, int& sign, Number& m);//Number - Number

    void alignNumbers(Number& lhs, Number& rhs, bool forward = true);//将两个Number对齐

    uint64_t removeZero(Number& trg, bool forward);//删去前/后多余的零

}  // namespace OperatorHelper

```

### Input.cpp

```c++
#include "Input.hpp"
#include "OperatorHelper.hpp"
#include "Operators.hpp"

#include <array>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <vector>

Input::Input() : inputString(), lowPriorityIndex(), highPriorityIndex(), sqrtIndex(), powIndex(), braces() {}

std::istream& operator>>(std::istream& i, Input& trg) {
    trg.inputString.clear();
    std::string tmpString;
    do {
        i >> tmpString;
        trg.inputString += tmpString;
    } while(std::cin.get() != '\n');
    return i;
}

std::unique_ptr<OperatorBase> Input::produce() {//处理读入的字符串
    try {
        if(inputString == "\n")
            throw InputException("empty_input");
        else if(inputString == "exit")
            throw InputException("exit");
        else if(inputString == "help") {
            std::cout << MANUAL;
            throw InputException("help");
        }

        for(int i = 0; i != inputString.length(); ++i) {//校验非法字符
            const char current = inputString[i];
            if('0' > current || '9' < current) {
                if(!std::count(ValidOperators.begin(), ValidOperators.end(), current)) {
                    //                    if(i + 3 < inputString.length()) {
                    //                        std::string tmpString(inputString.substr(i, 4));
                    //                        if("pow(" == tmpString) {
                    //                            i += 3;
                    //                            continue;
                    //                        } else {
                    if(i + 4 < inputString.length()) {
                        //   tmpString.clear();
                        auto tmpString = inputString.substr(i, 5);
                        if("sqrt(" == tmpString) {
                            i += 4;
                            continue;
                        }
                    }
                    //                        }
                    //                    }
                    throw InputException("Invalid input", inputString.substr(i));
                }
            }
        }
        if(std::count(inputString.begin(), inputString.end(), '='))
            throw InputException("Equation");
        else
            return std::unique_ptr<OperatorBase>(genOperators(inputString));
    } catch(InputException& e) {
        throw std::move(e);
    }
}

static std::vector<std::pair<int, int>> getRange(const std::vector<int>& vec, const std::function<bool(int)>& func) {//记录各符号位置
    std::vector<std::pair<int, int>> res;
    int beg = -1;
    for(int i = 0; i != vec.size(); ++i) {
        if(func(vec[i])) {
            if(beg == -1) {
                beg = i;
                res.emplace_back(i, -1);
            }
        } else if(!res.empty() && res.back().second == -1) {
            res.back().second = i;
            beg = -1;
        }
    }
    if(res.back().second == -1)
        res.back().second = vec.size();
    return res;
}

static std::vector<int> getInRange(const std::vector<int>& vec, const std::function<bool(int)>& func) {//使用static将函数限定为本文件访问(匿名名称空间也行但是容易找不着哪开始哪结束)
    std::vector<int> res;
    for(int index : vec) {
        if(func(index))
            res.push_back(index);
    }
    return res;
}

static Num searchNum(const std::string& trg, const int beg, const int end) {
    int sign = 0, eSign = 0;
    std::array<Number, 3> vecAry{ Number(), Number(), Number() };
    if(trg[beg] == '-')
        sign = 1;
    else if(trg[beg] != '+')
        vecAry[0].push_back(trg[beg] - 48);
    for(int i = 1 + beg, status = 0; i != end; ++i) {
        // 0 integer part; 1 float part; 2 e part
        if('.' == trg[i])
            status = 1;
        else if('e' == trg[i] || 'E' == trg[i])
            status = 2;
        else if('-' == trg[i]) {
            if(status == 2)
                eSign = 1;
            else
                throw InputException("Invalid input", trg.substr(i));
        } else if('+' == trg[i]) {
        } else {
            vecAry[status].push_back(trg[i] - 48);
        }
    }
    return Num(vecAry, sign, eSign);
}

static void genSingleOperator(const std::vector<int>& operatorsPosition,
                              std::vector<std::pair<std::array<std::int64_t, 2>, OperatorBase*>>& usedTable,
                              const std::string& oriString, const int index, const int end, bool haveOperator = true) {//生成单个运算符

    OperatorBase::OneSide lSide, rSide;
    std::int64_t lOper = -1, rOper = -1, newBeg = -1, newEnd = -1;
    char curOperator = oriString[index];
    if(!haveOperator) {
        lSide = new Num(std::move(searchNum(oriString, index, end)));
        newBeg = index;
        newEnd = end;
    } else if(curOperator != 's' && curOperator != 'p') {
        int tmpBackIndex = index, tmpFrontIndex = index;
        while(tmpBackIndex < oriString.length() && oriString[++tmpBackIndex] == '(')
            ;
        while(tmpFrontIndex > -1 && oriString[--tmpFrontIndex] == ')')
            ;
        for(int i = 0; i != usedTable.size(); ++i) {
            if(usedTable[i].first[0] == tmpBackIndex) {
                rSide = usedTable[i].second;
                rOper = i;
            } else if(usedTable[i].first[1] == tmpFrontIndex) {
                lSide = usedTable[i].second;
                lOper = i;
            }
        }
        auto curOperIter = std::find(operatorsPosition.begin(), operatorsPosition.end(), index);
        if(lOper == -1) {
            if(*curOperIter == operatorsPosition.front())
                newBeg = 0;
            else
                newBeg = *(curOperIter - 1) + 1;
            while(oriString[newBeg] == '(')
                ++newBeg;

            lSide = new Num(std::move(searchNum(oriString, newBeg, index)));
        }
        if(rOper == -1) {
            if(*curOperIter == operatorsPosition.back())
                newEnd = oriString.length() - 1;  // May overflow
            else
                newEnd = *(curOperIter + 1) - 1;
            while(oriString[newEnd] == ')')
                --newEnd;
            ++newEnd;
            rSide = new Num(std::move(searchNum(oriString, 1 + index, newEnd)));
        }
    } else if(curOperator == 's') {
        for(int i = 0; i != usedTable.size(); ++i) {
            if(usedTable[i].first[0] == index + 5) {
                rSide = usedTable[i].second;
                rOper = i;
            }
        }
        newBeg = index;
        newEnd = usedTable[rOper].first[1];
        lSide = usedTable[rOper].second;
    } else if(curOperator == 'p') {  // TODO:maybe give up pow implement
    }
    OperatorBase* ptrReplace;
    if(!haveOperator)
        ptrReplace = new NumWapper(lSide);
    else
        switch(curOperator) {
            case '+':
                ptrReplace = new Plus(lSide, rSide);
                break;
            case '-':
                ptrReplace = new Minus(lSide, rSide);
                break;
            case '*':
                ptrReplace = new Multiply(lSide, rSide);
                break;
            case '/':
                ptrReplace = new Divide(lSide, rSide);
                break;
            case 's':
                ptrReplace = new Sqrt(lSide);
                break;
            case 'p':
                ptrReplace = new Pow(lSide, rSide);
                break;
            default:
                for(int i = 0; i != usedTable.size(); ++i)
                    if(i != lOper && i != rOper)
                        delete usedTable[i].second;
                OperatorHelper::deleteSide(lSide);
                OperatorHelper::deleteSide(rSide);
                throw InputException("Invalid input", oriString.substr(index));
        }
    if(lOper != -1 && rOper != -1) {
        usedTable[lOper].first[1] = usedTable[rOper].first[1];
        usedTable[lOper].second = ptrReplace;
        usedTable.erase(usedTable.begin() + rOper);
    } else if(lOper != -1) {
        usedTable[lOper].first[1] = newEnd - 1;
        usedTable[lOper].second = ptrReplace;
    } else if(rOper != -1) {
        usedTable[rOper].first[0] = newBeg;
        usedTable[rOper].second = ptrReplace;
    } else {
        usedTable.emplace_back(std::array<std::int64_t, 2>{ newBeg, newEnd - 1 }, ptrReplace);
    }
}

static bool isNumberChar(char c) {
    return c >= '0' && c <= '9';
}

OperatorBase* Input::genOperators(const std::string& oriString) {
    initAll();

    char tmpChar;
    try {
        for(int i = 0; i != oriString.length(); ++i) {
            tmpChar = oriString[i];
            if('9' < tmpChar || '0' > tmpChar) {

                if(i == oriString.length() - 1 && tmpChar != ')')
                    throw oriString.substr(i);
                else if('+' == tmpChar || '-' == tmpChar) {
                    if(i == 0)
                        continue;
                    char preChar = oriString[i - 1];
                    bool preIsOper = false;
                    if(i > 1) {
                        char prePreChar = oriString[i - 2];
                        if('(' == preChar) {
                            constexpr std::array<char, 5> Check{ '+', '-', '*', '/', '(' };
                            if(std::count(Check.begin(), Check.end(), prePreChar) || (prePreChar >= '0' && prePreChar <= '9'))
                                continue;
                            else
                                throw oriString.substr(i);
                        } else {
                            constexpr std::array<char, 7> Check{ '+', '-', '*', '/', 'E', 'e', ')' };
                            if(isNumberChar(preChar) || ')' == preChar)
                                preIsOper = true;
                            else if(!std::count(Check.begin(), Check.end(), preChar))
                                throw oriString.substr(i);
                        }
                    } else if('(' != oriString[0])
                        throw oriString.substr(i);
                    {
                        constexpr std::array<char, 5> Check{ '+', '-', 's', 'p', '(' };
                        char nextChar = oriString[i + 1];
                        if(preIsOper && (isNumberChar(nextChar) || std::count(Check.begin(), Check.end(), nextChar)))
                            lowPriorityIndex.push_back(i);
                    }

                } else if('(' == tmpChar) {
                    braces.emplace_back(i, -1);
                } else if(')' == tmpChar) {
                    if(braces.empty())
                        throw oriString.substr(i, oriString.length() - i);
                    else {
                        bool done = false;
                        for(int j = braces.size() - 1; j != -1; --j) {
                            if(-1 == braces[j].second) {
                                done = braces[j].second = i;
                                break;
                            }
                        }
                        if(!done)
                            throw oriString.substr(i, oriString.length() - i);
                    }
                } else if('s' == tmpChar) {
                    if(i != 0) {
                        char preChar = oriString[i - 1];
                        if(isNumberChar(preChar) || ')' == preChar)
                            throw oriString.substr(i);
                    }
                    sqrtIndex.push_back(i);
                    i += 3;
                } else if('p' == tmpChar) {
                    if(i != 0) {
                        char preChar = oriString[i - 1];
                        if(isNumberChar(preChar) || ')' == preChar)
                            throw oriString.substr(i);
                    }
                    powIndex.push_back(i);
                    i += 2;
                } else if(i == 0) {
                    throw oriString;
                } else if('.' == tmpChar) {
                    if(!(isNumberChar(oriString[i - 1]) && isNumberChar(oriString[i + 1])))
                        throw oriString.substr(i);
                } else if('e' == tmpChar || 'E' == tmpChar) {
                    constexpr std::array<char, 2> Check{ '+', '-' };
                    if(!(isNumberChar(oriString[i - 1]) &&
                         (isNumberChar(oriString[i + 1]) || std::count(Check.begin(), Check.end(), oriString[i + 1]))))
                        throw oriString.substr(i);
                } else if('*' == tmpChar || '/' == tmpChar) {
                    if(!(')' == oriString[i - 1] || isNumberChar(oriString[i - 1])))
                        throw oriString.substr(i);
                    if(constexpr std::array<char, 5> Check{ '+', '-', 's', 'p', '(' };
                       !(std::count(Check.begin(), Check.end(), oriString[i + 1]) || isNumberChar(oriString[i + 1])))
                        throw oriString.substr(i);
                    highPriorityIndex.push_back(i);
                } else if(',' == tmpChar) {
                    commaIndex.push_back(i);
                } else
                    throw InputException("Unknown exception", oriString.substr(i));
            }
        }
        if(std::count_if(braces.begin(), braces.end(), [](const std::pair<int, int>& p) { return p.second == -1; }))
            throw InputException("Unmatched brace");

        std::vector<int> priority(oriString.length());
        std::fill(priority.begin(), priority.end(), 0);
        if(!braces.empty())
            for(auto [b, e] : braces) {
                std::fill(priority.begin() + b, priority.begin() + e, priority[b + 1] + 1);
                priority[b] = priority[e] = -1;
            }

        int maxPriority = *std::max_element(priority.begin(), priority.end());

#ifdef CAL_DEBUG
//        for(int atom : priority)
//            std::cout << atom;
//        std::cout << std::endl << "Max priority = " << maxPriority << std::endl;
#endif

        std::vector<int> operatorsPosition;
        const auto qCopy = [&operatorsPosition](const std::vector<int>& ori) {
            std::for_each(ori.begin(), ori.end(), [&operatorsPosition](int ind) { operatorsPosition.push_back(ind); });
        };
        qCopy(highPriorityIndex);
        qCopy(lowPriorityIndex);
        qCopy(powIndex);
        qCopy(sqrtIndex);
        qCopy(commaIndex);
        std::sort(operatorsPosition.begin(), operatorsPosition.end(), [](int lhs, int rhs) { return lhs < rhs; });
        std::vector<std::pair<std::array<std::int64_t, 2>, OperatorBase*>> numUseageTable;
        for(int i = maxPriority; i != -1; --i) {
            const auto range = getRange(priority, [i](int n) { return i == n; });
            if(!range.empty()) {
                bool isPow = false;
                for(auto [b, e] : range) {
                    int beg = b, end = e;
                    auto higherIndex = getInRange(highPriorityIndex, [beg, end](int n) { return beg <= n && n <= end; });
                    auto lowerIndex = getInRange(lowPriorityIndex, [beg, end](int n) { return beg <= n && n <= end; });
                    auto poIndex = getInRange(powIndex, [beg, end](int n) { return beg <= n && n <= end; });
                    auto sqrIndex = getInRange(sqrtIndex, [beg, end](int n) { return beg <= n && n <= end; });
                    auto comIndex = getInRange(commaIndex, [beg, end](int n) { return beg <= n && n <= end; });
                    for(const int pos : sqrIndex)
                        genSingleOperator(operatorsPosition, numUseageTable, oriString, pos, end);
                    if(isPow) {
                        for(const int pos : poIndex)
                            genSingleOperator(operatorsPosition, numUseageTable, oriString, pos, end);
                        isPow = false;
                    }
                    for(const int pos : higherIndex)
                        genSingleOperator(operatorsPosition, numUseageTable, oriString, pos, end);
                    for(const int pos : lowerIndex)
                        genSingleOperator(operatorsPosition, numUseageTable, oriString, pos, end);
                    if(!commaIndex.empty())
                        isPow = true;
                    if(higherIndex.empty() && lowerIndex.empty() && poIndex.empty() && sqrIndex.empty())
                        genSingleOperator(operatorsPosition, numUseageTable, oriString, beg, end, false);
                }
            }
        }

        return numUseageTable[0].second;
    } catch(std::string& s) {
        throw InputException("Invalid input", s);
    }
    return nullptr;
}

[[maybe_unused]] OperatorBase* Input::genEquation(const std::string& oriString) {
    // TODO: complete function
    return nullptr;
}

void Input::initAll() {
    lowPriorityIndex.clear();
    highPriorityIndex.clear();
    sqrtIndex.clear();
    powIndex.clear();
    commaIndex.clear();
    braces.clear();
}

InputException::InputException(std::string reason, std::string rawString)
    : reason(std::move(reason)), rawString(std::move(rawString)) {}

InputException::InputException(std::string reason) : reason(std::move(reason)), rawString() {}

```

## 结果展示

### 功能

![feature](https://github.com/uint44t/SUSTC_CS205-CPP/blob/main/project2/img/feature.png?raw=true)

### 非法输入检测

![error_input_detect](https://github.com/uint44t/SUSTC_CS205-CPP/blob/main/project2/img/error_input_detect.png?raw=true)

## 总结&心得

这次project花了不少时间，前期由于经验不足，不断地重构框架，由于对类继承的不熟悉导致各种编译错误频发； 但是经过了这次project的锻炼，巩固了之前在书本上学到的知识，也认识到了自己的漏洞，还是有很多收获的.
