#pragma once
#include <string>

constexpr int MAX_ENUM_COUNT = 100;
namespace SimpleEnum {
    template <class T, T n>
    std::string get_typename() {
        std::string s = __PRETTY_FUNCTION__;
        size_t _pos_b = s.find("n =");
        size_t _pos_e = s.find(';', _pos_b);
        _pos_b += 4;
        return s.substr(_pos_b, _pos_e - _pos_b);
    }

    template <class T>
    struct typename_hepler {
        T trg;
        std::string* s;

        typename_hepler(T trg, std::string* s) : trg(trg), s(s) {}

        template <int INDEX>
        void check() const {
            if(INDEX == trg)
                *s = get_typename<T, static_cast<T>(INDEX)>();
        }
    };

    template <int B>
    struct gen_check {};

    template <>
    struct gen_check<1> {
        typedef void type;
    };

    template <int BEG, int END, class F>
    typename gen_check<BEG == END>::type static_for(const F& stru) {}

    template <int BEG, int END, class F>
    typename gen_check<BEG != END>::type static_for(const F& stru) {
        stru.template check<BEG>();
        static_for<BEG + 1, END>(stru);
    }

    template <class T>
    std::string get_enum_name(T trg) {
        std::string s;
        static_for<0, MAX_ENUM_COUNT>(typename_hepler(trg, &s));
        if(s.empty())
            return "";

        return s;
    }

    template <class T>
    T enum_from_name(const std::string& s) {
        for(int i = 0; i != static_cast<int>(MAX_ENUM_COUNT); ++i) {
            if(get_enum_name(static_cast<T>(i)) == s)
                return static_cast<T>(i);
        }
        throw;
    }

}  // namespace SimpleEnum
