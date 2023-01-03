// implements
#define Temp_declare template <typename _T>
#define Mat_Iterator_temp Mat_Iterator<_T>
#define Mat_ConstIterator_temp Mat_ConstIterator<_T>

namespace rmat {

    // Mat_Iterator implements
    Temp_declare void Mat_Iterator_temp::movePtr(long long _n, Mat_Iterator_temp& src) {
        size_t absN = std::abs(_n);
        if(_n < 0 ^ src.forward) {
            long long sum = absN + src.colPos;
            src.data += (src.step + src.jump) * (sum / src.cols * src.ld + absN);
            src.colPos = sum % src.cols;
        } else {
            long long sum = absN - src.colPos;
            size_t crossTimes = (sum > 0 && sum < src.cols) ? 1 : sum / src.cols;
            if(sum < 0)
                crossTimes = 0;
            src.data -= (src.step + src.jump) * (crossTimes * src.ld + absN);
            src.colPos = (sum > 0 ? src.cols - sum % src.cols : -sum);
        }
    }

    Temp_declare Mat_Iterator_temp Mat_Iterator_temp::createMoved(long long _n, Mat_Iterator_temp& src) {
        size_t absN = std::abs(_n);
        if(_n < 0 ^ src.forward) {
            long long sum = absN + src.colPos;
            return Mat_Iterator_temp(src.data + (src.step + src.jump) * (sum / src.cols * src.ld + absN), src.step, src.jump,
                                     src.cols, sum % src.jump, src.ld, src.forward);
        } else {
            long long sum = absN - src.colPos;
            size_t crossTimes = (sum > 0 && sum < src.cols) ? 1 : sum / src.cols;
            if(sum < 0)
                crossTimes = 0;
            return Mat_Iterator_temp(src.data - (src.step + src.jump) * (crossTimes * src.ld + absN), src.step, src.jump,
                                     src.cols, (sum > 0 ? src.cols - sum % src.cols : -sum), src.ld, src.forward);
        }
    }

    Temp_declare Mat_Iterator_temp::Mat_Iterator(_T* data, size_t _channel, size_t _jump, size_t _cols, size_t _colPos,
                                                 size_t _ld, bool forward)
        : data(data), colPos(_colPos), step(_channel), jump(_jump), cols(_cols), ld(_ld), forward(forward) {
        if(_channel == 0 || _cols == 0 || !(_colPos < _cols))
            throw RM_MATSIZE_EXCEPTION();
    }

    Temp_declare _T& Mat_Iterator_temp::operator*() {
        return *data;
    }

    Temp_declare Mat_Iterator_temp Mat_Iterator_temp::operator+(long long _n) {
        return createMoved(_n, *this);
    }

    Temp_declare Mat_Iterator_temp& Mat_Iterator_temp::operator+=(long long _n) {
        movePtr(_n, *this);
        return *this;
    }

    Temp_declare Mat_Iterator_temp Mat_Iterator_temp::operator-(long long _n) {
        long long remain = _n - colPos;
        return createMoved(_n, *this);
    }

    Temp_declare Mat_Iterator_temp& Mat_Iterator_temp::operator-=(long long _n) {
        movePtr(_n, *this);
        return *this;
    }

    Temp_declare Mat_Iterator_temp& Mat_Iterator_temp::operator++() {
        movePtr(1, *this);
        return *this;
    }

    Temp_declare Mat_Iterator_temp Mat_Iterator_temp::operator++(int) {
        Mat_Iterator_temp res(*this);
        movePtr(1, *this);
        return res;
    }

    Temp_declare Mat_Iterator_temp& Mat_Iterator_temp::operator--() {
        movePtr(-1, *this);
        return *this;
    }

    Temp_declare Mat_Iterator_temp Mat_Iterator_temp::operator--(int) {
        Mat_Iterator_temp res(*this);
        movePtr(-1, *this);
        return res;
    }

    Temp_declare _T Mat_Iterator_temp::operator-(const Mat_Iterator_temp& rhs) const {
        return *data - *rhs.data;
    }

    Temp_declare _T& Mat_Iterator_temp::operator[](long long _n) {
        _T* dataSav = data;
        size_t posSav = colPos;
        movePtr(_n, *this);
        colPos = posSav;
        std::swap(data, dataSav);
        return *dataSav;
    }

    Temp_declare bool Mat_Iterator_temp::operator==(const Mat_Iterator_temp& rhs) const {
        return data == rhs.data;
    }

    Temp_declare bool Mat_Iterator_temp::operator!=(const Mat_Iterator_temp& rhs) const {
        return data != rhs.data;
    }

    Temp_declare bool Mat_Iterator_temp::operator<(const Mat_Iterator_temp& rhs) const {
        return *data < *rhs.data;
    }

    Temp_declare bool Mat_Iterator_temp::operator>(const Mat_Iterator_temp& rhs) const {
        return *data > *rhs.data;
    }

    Temp_declare bool Mat_Iterator_temp::operator<=(const Mat_Iterator_temp& rhs) const {
        return *data <= *rhs.data;
    }

    Temp_declare bool Mat_Iterator_temp::operator>=(const Mat_Iterator_temp& rhs) const {
        return *data >= *rhs.data;
    }

    // Mat_ConstIterator implements
    Temp_declare void Mat_ConstIterator_temp::movePtr(long long _n, Mat_ConstIterator_temp& src) {
        size_t absN = std::abs(_n);
        if(_n < 0 ^ src.forward) {
            long long sum = absN + src.colPos;
            src.data += (src.step + src.jump) * (sum / src.cols * src.ld + absN);
            src.colPos = sum % src.cols;
        } else {
            long long sum = absN - src.colPos;
            size_t crossTimes = (sum > 0 && sum < src.cols) ? 1 : sum / src.cols;
            if(sum < 0)
                crossTimes = 0;
            src.data -= (src.step + src.jump) * (crossTimes * src.ld + absN);
            src.colPos = (sum > 0 ? src.cols - sum % src.cols : -sum);
        }
    }

    Temp_declare Mat_ConstIterator_temp Mat_ConstIterator_temp::createMoved(long long _n, Mat_ConstIterator_temp& src) {
        size_t absN = std::abs(_n);
        if(_n < 0 ^ src.forward) {
            long long sum = absN + src.colPos;
            return Mat_ConstIterator_temp(src.data + (src.step + src.jump) * (sum / src.cols * src.ld + absN), src.step, src.jump,
                                          src.cols, sum % src.jump, src.ld, src.forward);
        } else {
            long long sum = absN - src.colPos;
            size_t crossTimes = (sum > 0 && sum < src.cols) ? 1 : sum / src.cols;
            if(sum < 0)
                crossTimes = 0;
            return Mat_ConstIterator_temp(src.data - (src.step + src.jump) * (crossTimes * src.ld + absN), src.step, src.jump,
                                          src.cols, (sum > 0 ? src.cols - sum % src.cols : -sum), src.ld, src.forward);
        }
    }

    Temp_declare Mat_ConstIterator_temp::Mat_ConstIterator(const _T* data, size_t _channel, size_t _jump, size_t _cols,
                                                           size_t _colPos, size_t _ld, bool forward)
        : data(data), colPos(_colPos), step(_channel), jump(_jump), cols(_cols), ld(_ld), forward(forward) {
        if(_channel == 0 || _cols == 0 || !(_colPos < _cols))
            throw RM_MATSIZE_EXCEPTION();
    }

    Temp_declare const _T& Mat_ConstIterator_temp::operator*() const {
        return *data;
    }

    Temp_declare Mat_ConstIterator_temp Mat_ConstIterator_temp::operator+(long long _n) const {
        return createMoved(_n, *this);
    }

    Temp_declare Mat_ConstIterator_temp& Mat_ConstIterator_temp::operator+=(long long _n) {
        movePtr(_n, *this);
        return *this;
    }

    Temp_declare Mat_ConstIterator_temp Mat_ConstIterator_temp::operator-(long long _n) const {
        return createMoved(_n, *this);
    }

    Temp_declare Mat_ConstIterator_temp& Mat_ConstIterator_temp::operator-=(long long _n) {
        movePtr(_n, *this);
        return *this;
    }

    Temp_declare Mat_ConstIterator_temp& Mat_ConstIterator_temp::operator++() {
        movePtr(1, *this);
        return *this;
    }

    Temp_declare Mat_ConstIterator_temp Mat_ConstIterator_temp::operator++(int) {
        Mat_ConstIterator_temp res(*this);
        movePtr(1, *this);
        return res;
    }

    Temp_declare Mat_ConstIterator_temp& Mat_ConstIterator_temp::operator--() {
        movePtr(-1, *this);
        return *this;
    }

    Temp_declare Mat_ConstIterator_temp Mat_ConstIterator_temp::operator--(int) {
        Mat_ConstIterator_temp res(*this);
        movePtr(-1, *this);
        return res;
    }

    Temp_declare _T Mat_ConstIterator_temp::operator-(const Mat_ConstIterator_temp& rhs) const {
        return *data - *rhs.data;
    }

    Temp_declare const _T& Mat_ConstIterator_temp::operator[](long long _n) {
        const _T* dataSav = data;
        size_t posSav = colPos;
        movePtr(_n, *this);
        colPos = posSav;
        std::swap(data, dataSav);
        return *dataSav;
    }

    Temp_declare bool Mat_ConstIterator_temp::operator==(const Mat_ConstIterator_temp& rhs) const {
        return data == rhs.data;
    }

    Temp_declare bool Mat_ConstIterator_temp::operator!=(const Mat_ConstIterator_temp& rhs) const {
        return data != rhs.data;
    }

    Temp_declare bool Mat_ConstIterator_temp::operator<(const Mat_ConstIterator_temp& rhs) const {
        return *data < *rhs.data;
    }

    Temp_declare bool Mat_ConstIterator_temp::operator>(const Mat_ConstIterator_temp& rhs) const {
        return *data > *rhs.data;
    }

    Temp_declare bool Mat_ConstIterator_temp::operator<=(const Mat_ConstIterator_temp& rhs) const {
        return *data <= *rhs.data;
    }

    Temp_declare bool Mat_ConstIterator_temp::operator>=(const Mat_ConstIterator_temp& rhs) const {
        return *data >= *rhs.data;
    }

}  // namespace rmat

#undef Temp_declare
#undef PointerWapper_temp
