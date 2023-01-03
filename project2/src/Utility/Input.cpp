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

std::unique_ptr<OperatorBase> Input::produce() {
    try {
        if(inputString == "\n")
            throw InputException("empty_input");
        else if(inputString == "exit")
            throw InputException("exit");
        else if(inputString == "help") {
            std::cout << MANUAL;
            throw InputException("help");
        }

        for(int i = 0; i != inputString.length(); ++i) {
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

static std::vector<std::pair<int, int>> getRange(const std::vector<int>& vec, const std::function<bool(int)>& func) {
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

static std::vector<int> getInRange(const std::vector<int>& vec, const std::function<bool(int)>& func) {
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
                              const std::string& oriString, const int index, const int end, bool haveOperator = true) {

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
