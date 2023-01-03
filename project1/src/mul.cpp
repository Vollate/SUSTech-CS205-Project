#include <algorithm>
#include <deque>
#include <iostream>
#include <memory>
#include <ostream>
#include <tuple>
#include <vector>

class Data {
public:
    enum NumType { Invalid, Valid };
    typedef std::vector<int> Num;

private:
    std::unique_ptr<std::tuple<Num, NumType>> storage;

public:
    Data(const char* inputPtr, const int length) {
        std::deque<int> eIndex, pointIndex, signIndex;
        Num res;
        const char* ptr = inputPtr;
        storage = std::make_unique<std::tuple<Num, NumType>>(std::make_tuple(Num(0), NumType::Invalid));
        for(int i = 0; i != length; ++i) {
            if('0' > ptr[i] || '9' < ptr[i]) {
                if((ptr[i] == 'e' || ptr[i] == 'E') && i != 0 && i != length - 1)
                    eIndex.push_back(i);
                else if(ptr[i] == '.')
                    pointIndex.push_back(i);
                else if(ptr[i] == '-')
                    signIndex.push_back(i);
                else
                    return;
            }
        }
        if(eIndex.size() > 1 || signIndex.size() > 2 || (eIndex.size() == 1 && pointIndex.size() != 1))
            return;

        int sign = 0;
        if(*ptr == '-')
            sign = 1;
        else if(*ptr != '0')
            std::get<Num>(*storage).push_back(*ptr - 48);
        if(eIndex.empty()) {
            if(pointIndex.empty())
                for(int i = 1; i != length; ++i)
                    std::get<Num>(*storage).push_back(ptr[i] - 48);
            else
                for(int i = 1; i != length; ++i)
                    if(i != pointIndex[0])
                        std::get<Num>(*storage).push_back(ptr[i] - 48);
            std::get<Num>(*storage).push_back(-1);
            std::get<Num>(*storage).push_back(
                pointIndex.empty() ? -1 : (eIndex.empty() ? length - (sign ? 0 : 1) : eIndex[0]) - pointIndex[0] - 1);
            std::get<Num>(*storage).push_back(sign);
            std::get<NumType>(*storage) = NumType::Valid;
            return;
        } else if(eIndex.size() == 1) {
            if(pointIndex[0] < eIndex[0]) {
                for(int i = 1; i != pointIndex[0]; ++i)
                    std::get<Num>(*storage).push_back(ptr[i] - 48);
                for(int i = pointIndex[0] + 1; i != eIndex[0]; ++i)
                    std::get<Num>(*storage).push_back(ptr[i] - 48);
                int eNum = 0;
                for(int i = eIndex[0] + 1; i != length; ++i)
                    eNum = eNum * 10 + (int)ptr[i] - 48;
                std::get<Num>(*storage).push_back(eNum);
                std::get<Num>(*storage).push_back(eIndex[0] - pointIndex[0] - 1);
                std::get<Num>(*storage).push_back(sign);
                std::get<NumType>(*storage) = NumType::Valid;
            } else
                return;
        }
    }

    Data(const Num& num, const NumType numType) {
        storage = std::make_unique<std::tuple<Num, NumType>>(std::make_tuple(Num(0), NumType::Invalid));
        std::get<Num>(*storage) = num;
        std::get<NumType>(*storage) = numType;
    }

    Data(const Data& data) {
        storage = std::make_unique<std::tuple<Num, NumType>>(std::make_tuple(Num(0), NumType::Invalid));
        std::get<Num>(*storage) = data.getNum();
        std::get<NumType>(*storage) = data.getNumType();
    }

    Data& operator=(const Data& data) {
        storage = std::make_unique<std::tuple<Num, NumType>>(std::make_tuple(Num(0), NumType::Invalid));
        std::get<Num>(*storage) = data.getNum();
        std::get<NumType>(*storage) = data.getNumType();
        return *this;
    }

    Data(Data&& data) noexcept {
        storage.swap(data.storage);
    }

    Data& operator=(Data&& data) noexcept {
        storage.swap(data.storage);
        return *this;
    }

    const Num& getNum() const {
        return std::get<Num>(*storage);
    }

    NumType getNumType() const {
        return std::get<NumType>(*storage);
    }

    friend std::ostream& operator<<(std::ostream& os, const Data& data);
};

std::ostream& operator<<(std::ostream& os, const Data& data) {
    auto& num = data.getNum();
    auto type = data.getNumType();
    int length, eSize, pointIndex;
    if(type == Data::NumType::Invalid)
        return os << "Invalid Num";
    length = num.size();
    pointIndex = num[length - 2];
    eSize = num[length - 3];
    if(num[num.size() - 1] == 1)
        os << '-';
    for(int i = 0; i != length - 3; ++i)
        os << num[i];
    if(eSize != -1 || pointIndex != -1)
        os << " * e" << (eSize + 1 ? 0 : eSize) - pointIndex;
    return os;
}

namespace Calculator {
    Data mul(const Data& lhs, const Data& rhs) {
        if(lhs.getNumType() == Data::NumType::Invalid || rhs.getNumType() == Data::NumType::Invalid)
            return Data(Data::Num(), Data::NumType::Invalid);
        const Data::Num &lhsNum = lhs.getNum(), rhsNum = rhs.getNum();
        Data::Num res(lhsNum.size());
        std::fill(res.begin(), res.end(), 0);
        for(int i = rhsNum.size() - 4, num = 0; i != -1; --i, ++num) {
            auto modify = lhs.getNum();
            for(int j = 0; j != num; ++j)
                modify.insert(modify.end() - 3, 0);
            for(int j = modify.size() - 4; j != -1; --j)
                modify[j] *= rhsNum[i];

            for(int j = modify.size() - 4; j != -1; --j) {
                int p = j;
                while(modify[p] > 9) {
                    if(modify[p] > 9) {
                        if(--p < 0)
                            modify.insert(modify.begin(), int(modify[++p] / 10));
                        else
                            modify[p] += int(modify[1 + p] / 10);
                        modify[p + 1] %= 10;
                    }
                }
            }
            while(res.size() < modify.size())
                res.insert(res.begin(), 0);
            for(int i = (int)modify.size() - 4, j = (int)res.size() - 4; i > -1; --i, --j) {
                res[j] += modify[i];
                int p = j;
                if(res[p] > 9) {
                    if(--p < 0)
                        res.insert(res.begin(), int(res[++p] / 10));
                    else
                        res[p] += int(res[1 + p] / 10);
                    res[p + 1] %= 10;
                }
            }
        }
        if(lhsNum[lhsNum.size() - 1] == rhsNum[rhsNum.size() - 1])
            res[res.size() - 1] = 0;
        else
            res[res.size() - 1] = 1;
        auto backwardGet = [](const Data::Num& ary, int index) -> int {
            int tmp = ary[ary.size() - index];
            return tmp == -1 ? 0 : tmp;
        };
        int tmp = backwardGet(lhsNum, 2) + backwardGet(rhsNum, 2);
        res[res.size() - 2] = tmp == 0 ? -1 : tmp;
        tmp = backwardGet(lhsNum, 3) + backwardGet(rhsNum, 3);
        res[res.size() - 3] = tmp == 0 ? -1 : tmp;
        return { res, Data::NumType::Valid };
    }
}  // namespace Calculator

int main(int argc, char* argv[]) {
    if(argc == 1) {
        std::endl(std::cout << "You input nothing with command line argument");
        return -1;
    } else if(argc == 2) {
        std::endl(std::cout << "Arguments are not enough");
        return -1;
    }

    std::vector<Data> mulData;
    for(int i = 1; i != argc; ++i) {
        unsigned int length = 0;
        const char* ptr = argv[i];
        char tempChar;
        do {
            tempChar = *ptr;
            ++ptr;
            ++length;
        } while(tempChar != '\0');
        mulData.emplace_back(argv[i], --length);
    }
    auto res = mulData[0];
    for(int i = 1; i != mulData.size(); ++i)
        res = std::move(Calculator::mul(res, mulData[i]));
    for(int i = 1; i != argc; ++i) {
        if(i - 1)
            std::cout << " * ";
        std::cout << argv[i];
    }
    std::endl(std::cout << " = " << res);
    return 0;
}
