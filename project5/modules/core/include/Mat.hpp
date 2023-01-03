#pragma once

#include "Mat_helper.hpp"

#include <algorithm>
#include <climits>
#include <cstring>
#include <exception>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#define Temp_declare_Mat_ template <typename _T>
#define curMat_ Mat_<_T>
#define RM_valid_row(_row) ((_row) >= 0 && (_row) < rows)
#define RM_valid_col(_col) ((_col) >= 0 && (_col) < cols)

namespace rmat {

    // forward declaration for friend functions
    Temp_declare_Mat_ class Mat_;

    Temp_declare_Mat_ std::ostream& operator<<(std::ostream& o, const Mat_<_T>& rhs);

    Temp_declare_Mat_ std::ostream& operator<<(std::ostream& o, Mat_<_T>&& rhs);

    //  Mat_ declaration
    template <typename _T>
    class Mat_ {
        std::shared_ptr<_T[]> dataBegin;
        _T* data;
        size_t rows, cols, channel;
        size_t realRows, realCols, realChannel;
        size_t rowPos, colPos, channelIdx;
        bool subMat, cpy;

        bool isSameSize(const Mat_<_T>& rhs, bool tran = false) const;

        explicit Mat_(size_t _rowBegin, size_t _colBegin, size_t _rows, size_t _cols, size_t _channelIdx, const Mat_& src,
                      bool hardCopy);

    public:
        // constructors
        Mat_() noexcept;
        Mat_(const curMat_& rhs) = default;
        Mat_(curMat_&& rhs) noexcept = default;
        Mat_(size_t _rows, size_t _cols, size_t _channel = 1);
        Mat_(size_t _rows, size_t _cols, const std::initializer_list<_T>& _val);
        Mat_(size_t _rows, size_t _cols, const std::vector<_T>& _val);

        Mat_& operator=(const Mat_& rhs) = default;
        Mat_& operator=(Mat_&& rhs) = default;

        // operators
        bool operator==(const Mat_& rhs) const;
        bool operator!=(const Mat_& rhs) const;
        Mat_ operator+(const Mat_& rhs) const;
        Mat_ operator-(const Mat_& rhs) const;
        Mat_ operator*(const Mat_& rhs) const;
        bool operator==(const Mat_&& rhs) const;
        bool operator!=(const Mat_&& rhs) const;
        Mat_ operator+(const Mat_&& rhs) const;
        Mat_ operator-(const Mat_&& rhs) const;
        Mat_ operator*(const Mat_&& rhs) const;
        friend std::ostream& operator<< <>(std::ostream& o, const Mat_& src);
        friend std::ostream& operator<< <>(std::ostream& o, Mat_&& src);

        // access
        size_t size() const;
        size_t max_size() const;
        bool empty() const;
        size_t rowSize() const;
        size_t colSize() const;
        size_t channelSize() const;
        size_t realRowSize() const;
        size_t realColSize() const;
        size_t realChannelSize() const;
        _T* ptr(size_t _row = 0);
        _T* ptr(size_t _row, size_t _col);
        const _T* ptr(size_t _row = 0) const;
        const _T* ptr(size_t _row, size_t _col) const;
        bool isContinuous() const;
        bool isSubMatrix() const;
        bool isCopy() const;
        Mat_Iterator<_T> begin();
        Mat_ConstIterator<_T> cbegin() const;
        Mat_Iterator<_T> rbegin();
        Mat_ConstIterator<_T> crbegin() const;
        Mat_Iterator<_T> end();
        Mat_ConstIterator<_T> cend() const;
        Mat_Iterator<_T> rend();
        Mat_ConstIterator<_T> crend() const;

        // others
        Mat_ getSubMat(size_t _rowBegin, size_t _colBegin, size_t _rows, size_t _cols, bool hardCopy = true) const;
        Mat_ getSubMat(size_t _rowBegin, size_t _colBegin, size_t _rows, size_t _cols, size_t _channelIdx, bool hardCopy) const;
        Mat_ getSplitedChannel(size_t _channelIdx, bool hardCopy) const;
        Mat_& splitMat(size_t _rowBegin, size_t _colBegin, size_t _rows, size_t _cols);
        Mat_& splitMat(size_t _rowBegin, size_t _colBegin, size_t _rows, size_t _cols, size_t _channelIdx);
        Mat_& splitChannel(size_t _channelIdx);
        std::vector<std::vector<_T>> getRow_v(size_t _row) const;
        std::vector<std::vector<_T>> getCol_v(size_t _col) const;
        Mat_ getRow(size_t _row, bool hardCopy = true);
        Mat_ getCol(size_t _row, bool hardCopy = true);
        Mat_ copy(bool hardCopy = true);
        void copy(Mat_& dest, bool hardCopy = true);
        void swap(Mat_& rhs) noexcept;
        friend void swap(Mat_& lhs, Mat_& rhs) noexcept;
        void clear();
        void realloc(size_t _rows, size_t _cols, size_t _channel);
    };

}  // namespace rmat

#include "Mat.inl"
#undef Temp_declare_Mat_
#undef curMat_
#undef RM_valid_row
#undef RM_valid_col
