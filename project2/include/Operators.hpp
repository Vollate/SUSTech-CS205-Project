#pragma once
#include "Num.hpp"

#include <memory>
#include <utility>
#include <variant>

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

template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

GENERATE_OPERATOR_DECLARATION(Plus)
GENERATE_OPERATOR_DECLARATION(Minus)
GENERATE_OPERATOR_DECLARATION(Multiply)
GENERATE_OPERATOR_DECLARATION(Divide)
GENERATE_OPERATOR_DECLARATION(Sqrt)
GENERATE_OPERATOR_DECLARATION(Pow)
GENERATE_OPERATOR_DECLARATION(NumWapper)
