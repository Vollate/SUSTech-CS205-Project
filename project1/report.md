# Project1: Simple Calculator

ID: 12111224  
Name: 贾禹帆

<!-- vim-markdown-toc Redcarpet -->

- [Basic Structure](#basic-structure)
    - [class Data](#class-data)
    - [namespace Calculator](#namespace-calculator)
        - [mul function](#mul-function)
    - [main function](#main-function)
- [Implementation](#implementation)
    - [Data](#data)
    - [mul](#mul)
    - [main](#main)
- [Test images](#test-images)
    - [Basic feature](#basic-feature)
    - [More than one number multiply](#more-than-one-number-multiply)

<!-- vim-markdown-toc -->
   **The code use google C++ code style and need at least C++14 standard to complie**

## Basic Structure

### class Data

- Using `vector<int>` to storage each digit. The last 3 digits storage the information of  `e`'s and `.`'s position (if not exist, use -1) and the num's sign(0 represents positive and 1 represents negative).  
- The constructor check whether the input is valid and use `enum NumType` to storage the input's validity.  
- To avoid stackoverflow, the data is storage in heap and managed by a smart pointer(`unique_ptr`).  
- The class define the copy constructor, move constructor, operator`=` for left value and right value.
- The class also define a friend function `std::ostream operator<<(std::ostream os, const Data& rhs)` to output the data.

### namespace Calculator

Using namespace to prevent global namespace pollute

#### mul function

`Data mul(const Data &lhs, const Data &rhs)`  

- Defined in namespace Calculator, get two Data reference in and return a Data
- Use high accuracy multiply(human multiply methord) to deal big num operator

### main function

- Get command arguments and do basic data check and produce
- Use `vector<Data>` storage num and do multiply
- Print out the result

## Implementation

### Data

Use unique_ptr manage memory, make num and number type to be a tuple

```c++
 public:
  enum NumType { Invalid, Valid };
  typedef std::vector<int> Num;

 private:
  std::unique_ptr<std::tuple<Num, NumType>> storage;
```

Constructor:

- Check every bit of the string, find the index of sign, "e" or "E" and point

    >The strings below are example that are invalid:  
    1. `123456e789` -- integer with scientific notation
    2. `1223infl` -- characters except "0~9", "-", "." and "e or E"
    3. `1234.35.34` or `123e2423e124` -- have more than  two "." or "e"
    4. `124-2134` -- "-" not at begining

```c++
Data(const char *inputPtr, const int length) {
 std::deque<int> eIndex, pointIndex, signIndex;
 Num res;
 const char *ptr = inputPtr;
 storage = std::make_unique<std::tuple<Num, NumType>>(std::make_tuple(Num(0), NumType::Invalid));
 for (int i = 0; i != length; ++i) {
   if ('0' > ptr[i] || '9' < ptr[i]) {
  if ((ptr[i] == 'e' || ptr[i] == 'E') && i != 0 && i != length - 1)
    eIndex.push_back(i);
  else if (ptr[i] == '.')
    pointIndex.push_back(i);
  else if (ptr[i] == '-')
    signIndex.push_back(i);
  else
    return;
   }
 }
```

- For each condition, do different operation

```c++
if (eIndex.size() > 1 || signIndex.size() > 2 || (eIndex.size() == 1 && pointIndex.size() != 1))
   return;

 int sign = 0;
 if (*ptr == '-')
   sign = 1;
 else if (*ptr != '0')
   std::get<Num>(*storage).push_back(*ptr - 48);
 if (eIndex.empty()) {
   if (pointIndex.empty())
  for (int i = 1; i != length; ++i)
    std::get<Num>(*storage).push_back(ptr[i] - 48);
   else
  for (int i = 1; i != length; ++i)
    if (i != pointIndex[0]) std::get<Num>(*storage).push_back(ptr[i] - 48);
   std::get<Num>(*storage).push_back(-1);
   std::get<Num>(*storage)
    .push_back(pointIndex.empty() ? -1 : (eIndex.empty() ? length - (sign ? 0 : 1) : eIndex[0])
    - pointIndex[0] - 1);
   std::get<Num>(*storage).push_back(sign);
   std::get<NumType>(*storage) = NumType::Valid;
   return;
 } else if (eIndex.size() == 1) {
   if (pointIndex[0] < eIndex[0]) {
  for (int i = 1; i != pointIndex[0]; ++i) std::get<Num>(*storage).push_back(ptr[i] - 48);
  for (int i = pointIndex[0] + 1; i != eIndex[0]; ++i)
    std::get<Num>(*storage).push_back(ptr[i] - 48);
  int eNum = 0;
  for (int i = eIndex[0] + 1; i != length; ++i) eNum = eNum * 10 + ptr[i] - 48;
  std::get<Num>(*storage).push_back(eNum);
  std::get<Num>(*storage).push_back(eIndex[0] - pointIndex[0] - 1);
  std::get<Num>(*storage).push_back(sign);
  std::get<NumType>(*storage) = NumType::Valid;
   } else
  return;
 }
}
```

Other constructors and assign operators:

```c++
  Data(const Num &num, const NumType numType) {
 storage = std::make_unique<std::tuple<Num, NumType>>(std::make_tuple(Num(0), NumType::Invalid));
 std::get<Num>(*storage) = num;
 std::get<NumType>(*storage) = numType;
  }

  Data(const Data &data) {
 storage = std::make_unique<std::tuple<Num, NumType>>(std::make_tuple(Num(0), NumType::Invalid));
 std::get<Num>(*storage) = data.getNum();
 std::get<NumType>(*storage) = data.getNumType();
  }

  Data &operator=(const Data &data) {
 storage = std::make_unique<std::tuple<Num, NumType>>(std::make_tuple(Num(0), NumType::Invalid));
 std::get<Num>(*storage) = data.getNum();
 std::get<NumType>(*storage) = data.getNumType();
 return *this;
  }

  Data(Data &&data) noexcept {
 storage.swap(data.storage);
  }

  Data &operator=(Data &&data) noexcept {
 storage.swap(data.storage);
 return *this;
  }
```

Two get function to get the Number Data handled

```c++
const Num& getNum() const {
    return std::get<Num>(*storage);
}

NumType getNumType() const {
    return std::get<NumType>(*storage);
}
```

### mul

Check numbers' validity, if any of the number is invalid, return result with `NumType::Invalid`

```c++
Data mul(const Data& lhs, const Data& rhs) {
  if (lhs.getNumType() == Data::NumType::Invalid || rhs.getNumType() == Data::NumType::Invalid)
    return Data(Data::Num(), Data::NumType::Invalid);
```

Multiply one digit of rhsNum with modify

```c++
 const Data::Num &lhsNum = lhs.getNum(), rhsNum = rhs.getNum();
  Data::Num res(lhsNum.size());
  std::fill(res.begin(), res.end(), 0);
  int add;
  for (int i = rhsNum.size() - 4, num = 0; i != -1; --i, ++num) {
    auto modify = lhs.getNum();
    for (int j = 0; j != num; ++j) modify.insert(modify.end() - 3, 0);
    for (int j = modify.size() - 4; j != -1; --j) modify[j] *= rhsNum[i];

    for (int j = modify.size() - 4; j != -1; --j) {
      while (modify[j] > 9) {
        add = modify[j] / 10;
        if (modify[j] % 10 == 0)
          modify[j] = 0;
        else
          modify[j] %= 10;
        if (--j < 0) {
          modify.insert(modify.begin(), add);
          ++j;
        } else
          modify[j] += add;
      }
    }
 ```

 Add modify to res

 ```c++
  while (res.size() < modify.size()) res.insert(res.begin(), 0);
    for (int i = (int)modify.size() - 4, j = (int)res.size() - 4; i > -1; --i, --j) {
      res[j] += modify[i];
      int p = j;
      if (res[p] > 9) {
        if (--p < 0)
          res.insert(res.begin(), 1);
        else
          res[p] += 1;
        res[j] -= 10;
      }
    }
  }
 ```

Add e, point index and signal digit for res and return

```c++
if (lhsNum[lhsNum.size() - 1] == rhsNum[rhsNum.size() - 1])
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
```

### Reload operator<< for Data

```c++
std::ostream &operator<<(std::ostream &os, const Data &data) {
  auto &num = data.getNum();
  auto type = data.getNumType();
  int length, eSize, pointIndex;
  if (type == Data::NumType::Invalid) return os << "Invalid Num";
  length = num.size();
  pointIndex = num[length - 2];
  eSize = num[length - 3];
  if (num[num.size() - 1] == 1) os << '-';
  for (int i = 0; i != length - 3; ++i) os << num[i];
  if (eSize != -1 || pointIndex != -1) os << " * e" << (eSize + 1 ? 0 : eSize) - pointIndex;
  return os;
}```

### main

Check if the input arguments is available

```c++
  if (argc == 1) {
    std::endl(std::cout << "You input nothing with command line argument");
    return -1;
  } else if (argc == 2) {
    std::endl(std::cout << "Arguments are not enough");
    return -1;
  }
```

Chech each string's length and call Data's constructor

```c++
 std::vector<Data> mulData;
  for (int i = 1; i != argc; ++i) {
 unsigned int length = 0;
 const char *ptr = argv[i];
 char tempChar;
 do {
   tempChar = *ptr;
   ++ptr;
   ++length;
 } while (tempChar != '\0');
 mulData.emplace_back(argv[i], --length);
  }
```

Call mul function and print the result

```c++
  auto res = mulData[0];
  for (int i = 1; i != mulData.size(); ++i)
 res = std::move(Calculator::mul(res, mulData[i]));
  for (int i = 1; i != argc; ++i) {
 if (i - 1)
   std::cout << " * ";
 std::cout << argv[i];
  }
  std::endl(std::cout << " = " << res);
  return 0;
}
```

## Test images

### Basic feature

![basic](https://raw.githubusercontent.com/uint44t/CS205-CPP/main/project1/image/basic.png)

### More than one number multiply

![enhance](https://raw.githubusercontent.com/uint44t/CS205-CPP/main/project1/image/mulNumber.png)
>Because the `e` digit is represented by int, it will overflow when the value is too large like below
![bang integer](https://raw.githubusercontent.com/uint44t/CS205-CPP/main/project1/image/overflow.png)
