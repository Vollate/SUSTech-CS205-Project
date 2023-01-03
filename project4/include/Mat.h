#include <stdlib.h>

#ifndef SIMPLE_MAT
#define SIMPLE_MAT

typedef struct fMat {
    size_t rows;
    size_t columns;
    float* data;
} fMat;

struct fMat createMatrix(size_t _rows, size_t _columns);

struct fMat makeNullMatrix();

void deleteMatrix(fMat* _in);

void copyMatrix(const fMat* _ori, fMat* _trg);

void addMatrix(const fMat* _lhs, const fMat* _rhs, fMat* _res);

void subtractMatrix(const fMat* _lhs, const fMat* _rhs, fMat* _res);

void addScalar(const fMat* _lhs, float _val, fMat* _res);

void subtractScalar(const fMat* _lhs, float _val, fMat* _res);

void multiplyMatrix(const fMat* _lhs, const fMat* _rhs, fMat* _res, int _method);

void matmul_plain(const fMat* _lhs, const fMat* _rhs, fMat* _res, int _method);

void matmul_improved(const fMat* _lhs, const fMat* _rhs, fMat* _res);

#define MATMUL_GENERATE 1
#define MATMUL_COPPERSMITHWINOGRAD 2

void multiplyScalar(const fMat* _lhs, float _val, fMat* _res);

void transposeMatrix(const fMat* _ori, fMat* _trg);

float getMax(const fMat* _mat);

float getMin(const fMat* _mat);

void printMat(const fMat* _ori, unsigned int _totalDigits, unsigned int _point_digits);

#define get(__mat, __row, __col) __mat.data[__row * __mat.columns + __col]

#endif
