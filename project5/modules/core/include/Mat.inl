// the functions' definition file

namespace rmat {

    // Mat_ definitions
    Temp_declare_Mat_ bool curMat_::isSameSize(const curMat_& rhs, bool tran) const {
        return tran ? (rows == rhs.cols && cols == rhs.rows) : (rows == rhs.rows && cols == rhs.cols);
    }

    Temp_declare_Mat_ curMat_::Mat_(size_t _rowBegin, size_t _colBegin, size_t _rows, size_t _cols, size_t _channelIdx,
                                    const curMat_& src, bool hardCopy)
        : rows(_rows), cols(_cols), channelIdx(_channelIdx == 0 ? src.channelIdx : _channelIdx),
          channel(_channelIdx == 0 ? src.channel : 1) {
        if(hardCopy) {
            subMat = cpy = false;
            realRows = rows, realCols = cols;
            rowPos = colPos = 0;
            dataBegin = std::shared_ptr<_T[]>(new _T[channel * rows * cols]());
            data = dataBegin.get();
            realChannel = channel;
            _T *dPtr = data, *sPtr = src.data;
            if(src.channelIdx == 0) {
                for(size_t i = 0; i < rows; ++i) {
                    std::memcpy(dPtr, sPtr, sizeof(_T) * channel * cols);
                    dPtr += cols;
                    sPtr += src.realCols;
                }
            } else {
                for(size_t i = 0; i < rows; ++i) {
                    _T* tmPtr = sPtr;
                    for(size_t j = 0; j < cols; ++j) {
                        *(dPtr++) = *tmPtr;
                        tmPtr += src.realChannel;
                    }
                    sPtr += src.realCols;
                }
            }
        } else {
            cpy = true;
            subMat = src.subMat || !(_rowBegin == 0 && _colBegin == 0 && rows == src.rows && cols == src.cols) || channelIdx != 0;
            realRows = src.realRows, realCols = src.realCols;
            rowPos = _rowBegin, colPos = _colBegin;
            realChannel = src.realChannel;
            dataBegin = src.dataBegin;
            if(_channelIdx == 0)
                data = src.data + src.realChannel * (rowPos * src.realCols + colPos);
            else
                data = src.data + src.realChannel * (rowPos * src.realCols + colPos) + channelIdx - 1;
        }
    }
    Temp_declare_Mat_ curMat_::Mat_() noexcept
        : dataBegin(nullptr), data(nullptr), realRows(0), realCols(0), realChannel(0), rowPos(0), colPos(0), channelIdx(0),
          subMat(false), cpy(false), rows(0), cols(0), channel(0) {}

    Temp_declare_Mat_ curMat_::Mat_(size_t _rows, size_t _cols, size_t _channel)
        : rows(_rows), cols(_cols), channel(_channel), realChannel(_channel), channelIdx(0), rowPos(0), colPos(0),
          realRows(_rows), realCols(_cols), cpy(false), subMat(false),
          dataBegin(_channel != 0 ? std::shared_ptr<_T[]>(new _T[_channel * _rows * _cols]()) : nullptr), data(dataBegin.get()) {
        if(_channel == 0)
            throw RM_CHANNEL_EXCEPTION();
    }

    Temp_declare_Mat_ curMat_::Mat_(size_t _rows, size_t _cols, const std::initializer_list<_T>& _val)
        : rows(_rows), cols(_cols), channel(_val.size()), realChannel(_val.size()), channelIdx(0), rowPos(0), colPos(0),
          realRows(_rows), realCols(_cols), cpy(false), subMat(false),
          dataBegin(_val.size() > 0 ? std::shared_ptr<_T[]>(new _T[_val.size() * _rows * _cols]()) : nullptr),
          data(dataBegin.get()) {
        if(_val.size() == 0)
            throw RM_INITLIZE_EXCEPTION();
        _T* p = data;
        const _T* src = std::data(_val);
        for(size_t i = 0; i < rows * cols; ++i) {
            std::memcpy(p, src, sizeof(_T) * channel);
            p += channel;
        }
    }

    Temp_declare_Mat_ curMat_::Mat_(size_t _rows, size_t _cols, const std::vector<_T>& _val)
        : rows(_rows), cols(_cols), channel(_val.size()), realChannel(_val.size()), channelIdx(0), rowPos(0), colPos(0),
          realRows(_rows), realCols(_cols), cpy(false), subMat(false),
          dataBegin(_val.size() > 0 ? std::shared_ptr<_T[]>(new _T[_val.size() * _rows * _cols]()) : nullptr),
          data(dataBegin.get()) {
        if(_val.size() == 0)
            throw RM_INITLIZE_EXCEPTION();
        _T* p = data;
        const _T* src = std::data(_val);
        for(size_t i = 0; i < rows * cols; ++i) {
            std::memcpy(p, src, sizeof(_T) * channel);
            p += channel;
        }
    }

    Temp_declare_Mat_ bool curMat_::operator==(const curMat_& rhs) const {
        if(!isSameSize(rhs) || channel != rhs.channel)
            return false;
        if(!subMat && !rhs.subMat) {
            const _T *cLhs = data, *cRhs = rhs.data;
            for(size_t i = 0; i < rows * cols; ++i)
                if(*cLhs++ != *cRhs++)
                    return false;

        } else {
            Mat_ConstIterator<_T> cLhs = cbegin(), cRhs = rhs.cbegin();
            for(size_t i = 0; i < rows * cols; ++i)
                if(*cLhs++ != *cRhs++)
                    return false;
        }
        return true;
    }

    Temp_declare_Mat_ bool curMat_::operator==(const curMat_&& rhs) const {
        auto&& tmp = rhs;
        if(!isSameSize(tmp) || channel != tmp.channel)
            return false;
        if(!subMat && !tmp.subMat) {
            const _T *cLhs = data, *cRhs = tmp.data;
            for(size_t i = 0; i < rows * cols; ++i)
                if(*cLhs++ != *cRhs++)
                    return false;

        } else {
            Mat_ConstIterator<_T> cLhs = cbegin(), cRhs = tmp.cbegin();
            for(size_t i = 0; i < rows * cols; ++i)
                if(*cLhs++ != *cRhs++)
                    return false;
        }
        return true;
    }

    Temp_declare_Mat_ bool curMat_::operator!=(const curMat_& rhs) const {
        if(!isSameSize(rhs) || channel != rhs.channel)
            return true;
        if(!subMat && !rhs.subMat) {
            const _T *cLhs = data, *cRhs = rhs.data;
            for(size_t i = 0; i < rows * cols; ++i)
                if(*cLhs++ != *cRhs++)
                    return true;

        } else {
            Mat_ConstIterator<_T> cLhs = cbegin(), cRhs = rhs.cbegin();
            for(size_t i = 0; i < rows * cols; ++i)
                if(*cLhs++ != *cRhs++)
                    return true;
        }
        return false;
    }

    Temp_declare_Mat_ bool curMat_::operator!=(const curMat_&& rhs) const {
        auto&& tmp = rhs;
        if(!isSameSize(tmp) || channel != tmp.channel)
            return true;
        if(!subMat && !tmp.subMat) {
            const _T *cLhs = data, *cRhs = tmp.data;
            for(size_t i = 0; i < rows * cols; ++i)
                if(*cLhs++ != *cRhs++)
                    return true;

        } else {
            Mat_ConstIterator<_T> cLhs = cbegin(), cRhs = tmp.cbegin();
            for(size_t i = 0; i < rows * cols; ++i)
                if(*cLhs++ != *cRhs++)
                    return true;
        }
        return false;
    }

    Temp_declare_Mat_ curMat_ curMat_::operator+(const curMat_& rhs) const {
        if(!isSameSize(rhs))
            throw RM_MATSIZE_EXCEPTION();
        if(channel != rhs.channel)
            throw RM_CHANNEL_EXCEPTION();
        curMat_ res(rows, cols, channel);
        _T* resPtr = res.ptr();
        if(!subMat && !rhs.subMat) {
            const _T *lhsPtr = data, *rhsPtr = rhs.ptr();
            for(size_t i = 0; i < channel * rows * cols; ++i) {
                *resPtr++ = *lhsPtr++ + *rhsPtr++;
            }
        } else {
            Mat_ConstIterator<_T> lhsCIter = cbegin(), rhsCIter = rhs.cbegin();
            for(size_t i = 0; i < channel * rows * cols; ++i) {
                *resPtr++ = *lhsCIter++ + *rhsCIter++;
            }
        }
        return res;
    }

    Temp_declare_Mat_ curMat_ curMat_::operator+(const curMat_&& rhs) const {
        auto tmp(rhs);
        if(!isSameSize(tmp))
            throw RM_MATSIZE_EXCEPTION();
        if(channel != tmp.channel)
            throw RM_CHANNEL_EXCEPTION();
        curMat_ res(rows, cols, channel);
        _T* resPtr = res.ptr();
        if(!subMat && !tmp.subMat) {
            const _T *lhsPtr = data, *rhsPtr = tmp.ptr();
            for(size_t i = 0; i < channel * rows * cols; ++i) {
                *resPtr++ = *lhsPtr++ + *rhsPtr++;
            }
        } else {
            Mat_ConstIterator<_T> lhsCIter = cbegin(), rhsCIter = tmp.cbegin();
            for(size_t i = 0; i < channel * rows * cols; ++i) {
                *resPtr++ = *lhsCIter++ + *rhsCIter++;
            }
        }
        return res;
    }

    Temp_declare_Mat_ curMat_ curMat_::operator-(const curMat_& rhs) const {
        if(!isSameSize(rhs))
            throw RM_MATSIZE_EXCEPTION();
        if(channel != rhs.channel)
            throw RM_CHANNEL_EXCEPTION();
        curMat_ res(rows, cols, channel);
        _T* resPtr = res.ptr();
        Mat_ConstIterator<_T> lhsCIter = cbegin(), rhsCIter = rhs.cbegin();
        if(!subMat && !rhs.subMat) {
            const _T *lhsPtr = data, *rhsPtr = rhs.ptr();
            for(size_t i = 0; i < channel * rows * cols; ++i) {
                *(resPtr++) = *(lhsPtr++) - *(rhsPtr++);
            }
        } else {
            Mat_ConstIterator<_T> lhsCIter = cbegin(), rhsCIter = rhs.cbegin();
            for(size_t i = 0; i < channel * rows * cols; ++i) {
                *(resPtr++) = *lhsCIter++ - *rhsCIter++;
            }
        }
        return res;
    }

    Temp_declare_Mat_ curMat_ curMat_::operator-(const curMat_&& rhs) const {
        auto tmp(rhs);
        if(!isSameSize(tmp))
            throw RM_MATSIZE_EXCEPTION();
        if(channel != tmp.channel)
            throw RM_CHANNEL_EXCEPTION();
        curMat_ res(rows, cols, channel);
        _T* resPtr = res.ptr();
        Mat_ConstIterator<_T> lhsCIter = cbegin(), rhsCIter = tmp.cbegin();
        if(!subMat && !tmp.subMat) {
            const _T *lhsPtr = data, *rhsPtr = tmp.ptr();
            for(size_t i = 0; i < channel * rows * cols; ++i) {
                *(resPtr++) = *(lhsPtr++) - *(rhsPtr++);
            }
        } else {
            Mat_ConstIterator<_T> lhsCIter = cbegin(), rhsCIter = tmp.cbegin();
            for(size_t i = 0; i < channel * rows * cols; ++i) {
                *(resPtr++) = *lhsCIter++ - *rhsCIter++;
            }
        }
        return res;
    }

    Temp_declare_Mat_ curMat_ curMat_::operator*(const curMat_& rhs) const {
        if(!(cols == rhs.rows))
            throw RM_MATSIZE_EXCEPTION();
        if(channel != rhs.channel)
            throw RM_CHANNEL_EXCEPTION();
        curMat_ res(rows, rhs.cols, channel);
        if(!subMat && !rhs.subMat) {
            _T *lhsPtr = data, *rhsPtr = rhs.data, *resPtr = res.data;
            for(size_t i = 0; i < rows; ++i) {
                for(size_t j = 0; j < rhs.cols; ++j) {
                    for(size_t k = 0; k < cols; ++k) {
                        _T* tmpRes = resPtr;
                        for(size_t l = 0; l < channel; ++l) {
                            *tmpRes++ += lhsPtr[(i * cols + k) * channel + l] * rhsPtr[(k * rhs.cols + j) * channel + l];
                        }
                    }
                    resPtr += channel;
                }
            }
        } else {
            auto lhsCIter = cbegin(), rhsCIter = rhs.cbegin();
            _T* resPtr = res.data;
            for(size_t i = 0; i < rows; ++i) {
                for(size_t j = 0; j < rhs.cols; ++j) {
                    for(size_t k = 0; k < cols; ++k) {
                        _T* tmpRes = resPtr;
                        for(size_t l = 0; l < channel; ++l) {
                            *tmpRes++ += *(&lhsCIter[i * cols + k] + l) * *(&rhsCIter[k * rhs.cols + j]);
                        }
                    }
                    resPtr += channel;
                }
            }
        }
        return res;
    }

    Temp_declare_Mat_ curMat_ curMat_::operator*(const curMat_&& rhs) const {
        auto tmp(rhs);
        if(!(cols == tmp.rows))
            throw RM_MATSIZE_EXCEPTION();
        if(channel != tmp.channel)
            throw RM_CHANNEL_EXCEPTION();
        curMat_ res(rows, tmp.cols, channel);
        if(!subMat && !tmp.subMat) {
            _T *lhsPtr = data, *rhsPtr = tmp.data, *resPtr = res.data;
            for(size_t i = 0; i < rows; ++i) {
                for(size_t j = 0; j < tmp.cols; ++j) {
                    for(size_t k = 0; k < cols; ++k) {
                        _T* tmpRes = resPtr;
                        for(size_t l = 0; l < channel; ++l) {
                            *tmpRes++ += lhsPtr[(i * cols + k) * channel + l] * rhsPtr[(k * tmp.cols + j) * channel + l];
                        }
                    }
                    resPtr += channel;
                }
            }
        } else {
            auto lhsCIter = cbegin(), rhsCIter = tmp.cbegin();
            _T* resPtr = res.data;
            for(size_t i = 0; i < rows; ++i) {
                for(size_t j = 0; j < tmp.cols; ++j) {
                    for(size_t k = 0; k < cols; ++k) {
                        _T* tmpRes = resPtr;
                        for(size_t l = 0; l < channel; ++l) {
                            *tmpRes++ += *(&lhsCIter[i * cols + k] + l) * *(&rhsCIter[k * rhs.cols + j]);
                        }
                    }
                    resPtr += channel;
                }
            }
        }
        return res;
    }

    Temp_declare_Mat_ std::ostream& operator<<(std::ostream& o, const curMat_& rhs) {
        auto cIter = rhs.cbegin();
        for(size_t i = 0; i < rhs.rows; ++i) {
            for(size_t j = 0; j < rhs.cols; ++j) {
                o << "[ ";
                auto tmp = cIter.data;
                for(size_t k = 0; k < rhs.channel - 1; ++k)
                    o << *(tmp++) << ", ";
                o << *(tmp) << " ]";
            }
            std::endl(std::cout);
        }
        return o;
    }

    Temp_declare_Mat_ std::ostream& operator<<(std::ostream& o, curMat_&& rhs) {
        curMat_ xMat(rhs);
        auto cIter = xMat.cbegin();
        for(size_t i = 0; i < xMat.rows; ++i) {
            for(size_t j = 0; j < xMat.cols; ++j) {
                o << "[ ";
                auto tmp = cIter.data;
                for(size_t k = 0; k < xMat.channel - 1; ++k)
                    o << *(tmp++) << ", ";
                o << *(tmp) << " ]";
            }
            std::endl(std::cout);
        }
        return o;
    }
    Temp_declare_Mat_ size_t curMat_::size() const {
        return rows * cols;
    }

    Temp_declare_Mat_ size_t curMat_::max_size() const {
        return UINT64_MAX;
    }

    Temp_declare_Mat_ bool curMat_::empty() const {
        return data == nullptr;
    }

    Temp_declare_Mat_ size_t curMat_::rowSize() const {
        return rows;
    }

    Temp_declare_Mat_ size_t curMat_::colSize() const {
        return cols;
    }

    Temp_declare_Mat_ size_t curMat_::channelSize() const {
        return channel;
    }

    Temp_declare_Mat_ size_t curMat_::realRowSize() const {
        return realRows;
    }

    Temp_declare_Mat_ size_t curMat_::realColSize() const {
        return realCols;
    }

    Temp_declare_Mat_ size_t curMat_::realChannelSize() const {
        return realChannel;
    }

    Temp_declare_Mat_ _T* curMat_::ptr(size_t _row) {
        if(!RM_valid_row(_row))
            throw RM_MATSIZE_EXCEPTION();
        return data + realChannel * _row * (subMat ? realCols : cols);
    }

    Temp_declare_Mat_ _T* curMat_::ptr(size_t _row, size_t _col) {
        if(!(RM_valid_row(_row) && RM_valid_col(_col)))
            throw RM_MATSIZE_EXCEPTION();
        return data + realChannel * (_col + _row * (subMat ? realCols : cols));
    }

    Temp_declare_Mat_ const _T* curMat_::ptr(size_t _row) const {
        if(!RM_valid_row(_row))
            throw RM_MATSIZE_EXCEPTION();
        return data + realChannel * _row * (subMat ? realCols : cols);
    }

    Temp_declare_Mat_ const _T* curMat_::ptr(size_t _row, size_t _col) const {
        if(!(RM_valid_row(_row) && RM_valid_col(_col)))
            throw RM_MATSIZE_EXCEPTION();
        return data + realChannel * (_col + _row * (subMat ? realCols : cols));
    }

    Temp_declare_Mat_ bool curMat_::isContinuous() const {
        return !subMat || channelIdx == 0;
    }

    Temp_declare_Mat_ bool curMat_::isSubMatrix() const {
        return subMat;
    }

    Temp_declare_Mat_ bool curMat_::isCopy() const {
        return cpy;
    }

    Temp_declare_Mat_ Mat_Iterator_temp curMat_::begin() {
        return Mat_Iterator_temp(data, channel, realChannel - channel, cols, 0, realCols - cols, true);
    }

    Temp_declare_Mat_ Mat_ConstIterator_temp curMat_::cbegin() const {
        return Mat_ConstIterator_temp(data, channel, realChannel - channel, cols, 0, realCols - cols, true);
    }

    Temp_declare_Mat_ Mat_Iterator_temp curMat_::rbegin() {
        return Mat_Iterator_temp(data + realChannel * ((rows - 1) * realCols + cols - 1), channel, realChannel - channel, cols, 0,
                                 realCols - cols, false);
    }

    Temp_declare_Mat_ Mat_ConstIterator_temp curMat_::crbegin() const {
        return Mat_ConstIterator_temp(data + realChannel * ((rows - 1) * realCols + cols - 1), channel, realChannel - channel,
                                      cols, 0, realCols - cols, false);
    }

    Temp_declare_Mat_ Mat_Iterator_temp curMat_::end() {
        return Mat_Iterator_temp(data + realChannel * rows * realCols, channel, realChannel - channel, cols, 0, realCols - cols,
                                 true);
    }

    Temp_declare_Mat_ Mat_ConstIterator_temp curMat_::cend() const {
        return Mat_ConstIterator_temp(data + realChannel * rows * realCols, channel, realChannel - channel, cols, 0,
                                      realCols - cols, true);
    }

    Temp_declare_Mat_ Mat_Iterator_temp curMat_::rend() {
        return Mat_Iterator_temp(data - realChannel * (realCols - cols + 1), channel, realChannel - channel, cols, cols - 1,
                                 realCols - cols, false);
    }

    Temp_declare_Mat_ Mat_ConstIterator_temp curMat_::crend() const {
        return Mat_ConstIterator_temp(data - realChannel * (realCols - cols + 1), channel, realChannel - channel, cols, 0,
                                      realCols - cols, false);
    }

    Temp_declare_Mat_ curMat_ curMat_::getSubMat(size_t _rowBegin, size_t _colBegin, size_t _rows, size_t _cols,
                                                 bool hardCopy) const {
        if(!RM_valid_row(_rowBegin) || !RM_valid_col(_colBegin) || _rowBegin + _rows > rows || _colBegin + _cols > cols)
            throw RM_MATSIZE_EXCEPTION();
        return Mat_(_rowBegin, _colBegin, _rows, _cols, 0, *this, hardCopy);
    }

    Temp_declare_Mat_ curMat_ curMat_::getSubMat(size_t _rowBegin, size_t _colBegin, size_t _rows, size_t _cols,
                                                 size_t _channelIdx, bool hardCopy) const {
        if(!RM_valid_row(_rowBegin) || !RM_valid_col(_colBegin) || _rowBegin + _rows > rows || _colBegin + _cols > cols)
            throw RM_MATSIZE_EXCEPTION();
        return Mat_(_rowBegin, _colBegin, _rows, _cols, _channelIdx, *this, hardCopy);
    }

    Temp_declare_Mat_ curMat_ curMat_::getSplitedChannel(size_t _channelIdx, bool hardCopy) const {
        if(_channelIdx < 0 || _channelIdx > channel)
            throw RM_CHANNEL_EXCEPTION();
        return curMat_(0, 0, rows, cols, _channelIdx, *this, hardCopy);
    }

    Temp_declare_Mat_ curMat_& curMat_::splitMat(size_t _rowBegin, size_t _colBegin, size_t _rows, size_t _cols) {
        if(!RM_valid_row(_rowBegin) || !RM_valid_col(_colBegin) || _rowBegin + _rows > rows || _colBegin + _cols > cols)
            throw RM_MATSIZE_EXCEPTION();
        rowPos = _rowBegin, colPos = _colBegin;
        rows = _rows, cols = _cols;
        data = dataBegin.get() + realChannel * (rowPos * realCols + colPos);
        subMat = (subMat || rowPos != 0 || colPos != 0 || rows != realRows || cols != realCols);
        return *this;
    }

    Temp_declare_Mat_ curMat_& curMat_::splitMat(size_t _rowBegin, size_t _colBegin, size_t _rows, size_t _cols,
                                                 size_t _channelIdx) {
        if(!RM_valid_row(_rowBegin) || !RM_valid_col(_colBegin) || _rowBegin + _rows > rows || _colBegin + _cols > cols)
            throw RM_MATSIZE_EXCEPTION();
        channelIdx = _channelIdx;
        if(channelIdx != 0)
            channel = 1;
        rowPos = _rowBegin, colPos = _colBegin;
        rows = _rows, cols = _cols;
        if(channelIdx == 0)
            data = dataBegin.get() + realChannel * (rowPos * realCols + colPos);
        else
            data = dataBegin.get() + realChannel * (rowPos * realCols + colPos) + channelIdx - 1;
        if(!channelIdx)
            data += channelIdx - 1;
        subMat = (subMat || rowPos != 0 || colPos != 0 || rows != realRows || cols != realCols || channelIdx != 0);
        return *this;
    }

    Temp_declare_Mat_ curMat_& curMat_::splitChannel(size_t _channelIdx) {
        if(_channelIdx > channel)
            throw RM_CHANNEL_EXCEPTION();
        channelIdx = _channelIdx;
        if(channelIdx != 0)
            channel = 1;
        if(channelIdx > 1)
            data += channelIdx - 1;
        subMat = (subMat || channelIdx != 0);
        return *this;
    }

    Temp_declare_Mat_ std::vector<std::vector<_T>> curMat_::getRow_v(size_t _row) const {
        if(!RM_valid_row(_row))
            throw RM_MATSIZE_EXCEPTION();
        std::vector<std::vector<_T>> res;
        if(subMat) {
            auto iter = cbegin();
            iter += _row * cols;
            for(size_t i = 0; i < cols; ++i) {
                res.emplace_back(std::vector<_T>());
                auto tmp = res.back();
                for(size_t j = 0; j < channel; ++j)
                    tmp.push_back(*(iter++));
            }
        } else {
            const _T* ptr = data + realChannel * _row * realCols;
            for(size_t i = 0; i < cols; ++i) {
                res.emplace_back(std::vector<_T>());
                auto tmp = res.back();
                for(size_t j = 0; j < channel; ++j)
                    tmp.push_back(*(ptr++));
            }
        }
        return res;
    }

    Temp_declare_Mat_ std::vector<std::vector<_T>> curMat_::getCol_v(size_t _col) const {
        if(!RM_valid_col(_col))
            throw RM_MATSIZE_EXCEPTION();
        std::vector<std::vector<_T>> res;
        if(subMat) {
            auto iter = cbegin();
            iter += _col;
            for(size_t i = 0; i < rows; ++i) {
                res.emplace_back(std::vector<_T>());
                auto tmp = res.back();
                for(size_t j = 0; j < channel; ++j) {
                    tmp.push_back(*iter);
                    iter += cols;
                }
            }
        } else {
            const _T* ptr = data + realChannel * _col;
            for(size_t i = 0; i < cols; ++i) {
                res.emplace_back(std::vector<_T>());
                auto tmp = res.back();
                for(size_t j = 0; j < channel; ++j) {
                    tmp.push_back(*ptr);
                    ptr += realCols;
                }
            }
        }
        return res;
    }

    Temp_declare_Mat_ curMat_ curMat_::getRow(size_t _row, bool hardCopy) {
        if(!RM_valid_row(_row))
            throw RM_MATSIZE_EXCEPTION();
        return Mat_(_row, 0, 1, cols, 0, *this, hardCopy);
    }

    Temp_declare_Mat_ curMat_ curMat_::getCol(size_t _col, bool hardCopy) {
        if(!RM_valid_col(_col))
            throw RM_MATSIZE_EXCEPTION();
        return Mat_(0, _col, rows, cols, 0, *this, hardCopy);
    }

    Temp_declare_Mat_ curMat_ curMat_::copy(bool hardCopy) {
        return Mat_(0, 0, rows, cols, 0, *this, hardCopy);
    }

    Temp_declare_Mat_ void curMat_::copy(curMat_& dest, bool hardCopy) {
        dest = Mat_(0, 0, rows, 1, 0, *this, hardCopy);
    }

    Temp_declare_Mat_ void curMat_::swap(curMat_& rhs) noexcept {
#define auto_swap(_name) std::swap(_name, rhs._name)
        auto_swap(dataBegin);
        auto_swap(data);
        auto_swap(rows);
        auto_swap(cols);
        auto_swap(realRows);
        auto_swap(realCols);
        auto_swap(rowPos);
        auto_swap(colPos);
        auto_swap(channelIdx);
        auto_swap(subMat);
        auto_swap(cpy);
#undef auto_swap
    }

    Temp_declare_Mat_ void swap(curMat_& lhs, curMat_& rhs) noexcept {
#define auto_swap(_name) std::swap(lhs._name, rhs._name)
        auto_swap(dataBegin);
        auto_swap(data);
        auto_swap(rows);
        auto_swap(cols);
        auto_swap(realRows);
        auto_swap(realCols);
        auto_swap(rowPos);
        auto_swap(colPos);
        auto_swap(channelIdx);
        auto_swap(subMat);
        auto_swap(cpy);
#undef auto_swap
    }

    Temp_declare_Mat_ void curMat_::clear() {
        dataBegin = nullptr;
        data = nullptr;
        rows = cols = realRows = realCols = channel = channelIdx = 0;
        subMat = cpy = false;
    }

    Temp_declare_Mat_ void curMat_::realloc(size_t _rows, size_t _cols, size_t _channel) {
        dataBegin = std::shared_ptr<_T[]>(new _T[_rows * _cols * _channel]());
        data = dataBegin.get();
        rows = realRows = _rows;
        cols = realCols = _cols;
        channel = _channel;
        channelIdx = 0;
        subMat = cpy = false;
    }

}  // namespace rmat
