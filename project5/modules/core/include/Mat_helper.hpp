#pragma once

#include <cmath>
#include <cstddef>
#include <cstring>
#include <exception>
#include <utility>
#include <vector>

namespace rmat {

    template <typename _T>
    struct Mat_Iterator {
        _T* data;

    private:
        size_t step, jump, colPos, cols, ld;
        bool forward;

        static void movePtr(long long _n, Mat_Iterator& src);
        static Mat_Iterator createMoved(long long _n, Mat_Iterator& src);

    public:
        using difference_type = std::ptrdiff_t;
        using value_type = _T;
        using pointer = _T*;
        using reference = _T&;
        using iterator_category = std::random_access_iterator_tag;
        Mat_Iterator(_T* data, size_t _channel, size_t _jump, size_t _cols, size_t _colPos, size_t _ld, bool forward);
        Mat_Iterator(const Mat_Iterator& src) = default;
        Mat_Iterator(Mat_Iterator&& src) = default;
        Mat_Iterator& operator=(const Mat_Iterator& rhs) = default;
        Mat_Iterator& operator=(Mat_Iterator&& rhs) = default;
        _T& operator*();
        Mat_Iterator operator+(long long _n);
        Mat_Iterator& operator+=(long long _n);
        Mat_Iterator operator-(long long _n);
        Mat_Iterator& operator-=(long long _n);
        Mat_Iterator& operator++();
        Mat_Iterator operator++(int);
        Mat_Iterator& operator--();
        Mat_Iterator operator--(int);
        _T operator-(const Mat_Iterator& rhs) const;
        _T& operator[](long long _n);
        bool operator==(const Mat_Iterator& rhs) const;
        bool operator!=(const Mat_Iterator& rhs) const;
        bool operator<(const Mat_Iterator& rhs) const;
        bool operator>(const Mat_Iterator& rhs) const;
        bool operator<=(const Mat_Iterator& rhs) const;
        bool operator>=(const Mat_Iterator& rhs) const;
    };

    template <typename _T>
    struct Mat_ConstIterator {
        const _T* data;

    private:
        size_t step, jump, colPos, cols, ld;
        bool forward;

        static void movePtr(long long _n, Mat_ConstIterator& src);
        static Mat_ConstIterator createMoved(long long _n, Mat_ConstIterator& src);

    public:
        using difference_type = std::ptrdiff_t;
        using value_type = const _T;
        using pointer = const _T*;
        using reference = const _T&;
        using iterator_category = std::random_access_iterator_tag;
        Mat_ConstIterator(const _T* data, size_t _channel, size_t _jump, size_t _cols, size_t _colPos, size_t _ld, bool forward);
        Mat_ConstIterator(const Mat_ConstIterator& src) = default;
        Mat_ConstIterator(Mat_ConstIterator&& src) = default;
        Mat_ConstIterator& operator=(const Mat_ConstIterator& rhs) = default;
        Mat_ConstIterator& operator=(Mat_ConstIterator&& rhs) = default;
        const _T& operator*() const;
        Mat_ConstIterator operator+(long long _n) const;
        Mat_ConstIterator& operator+=(long long _n);
        Mat_ConstIterator operator-(long long _n) const;
        Mat_ConstIterator& operator-=(long long _n);
        Mat_ConstIterator& operator++();
        Mat_ConstIterator operator++(int);
        Mat_ConstIterator& operator--();
        Mat_ConstIterator operator--(int);
        _T operator-(const Mat_ConstIterator& rhs) const;
        const _T& operator[](long long _n);
        bool operator==(const Mat_ConstIterator& rhs) const;
        bool operator!=(const Mat_ConstIterator& rhs) const;
        bool operator<(const Mat_ConstIterator& rhs) const;
        bool operator>(const Mat_ConstIterator& rhs) const;
        bool operator<=(const Mat_ConstIterator& rhs) const;
        bool operator>=(const Mat_ConstIterator& rhs) const;
    };

    template <char const* c>
    class RM_Exception final : public std::exception {
    public:
        virtual const char* what() const noexcept override {
            return c;
        }
    };

    // exception define
    static constexpr char _wrong_initilize_size[] = "wrong initializer size";
    static constexpr char _wrong_mat_size[] = "wrong Mat_ size";
    static constexpr char _wrong_channel[] = "wrong channel";
    using RM_INITLIZE_EXCEPTION = RM_Exception<_wrong_initilize_size>;
    using RM_MATSIZE_EXCEPTION = RM_Exception<_wrong_mat_size>;
    using RM_CHANNEL_EXCEPTION = RM_Exception<_wrong_channel>;

}  // namespace rmat

#include "Mat_helper.inl"
