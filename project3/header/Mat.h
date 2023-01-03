#ifndef SIMPLE_MAT
#define SIMPLE_MAT

#include <stdlib.h>
#include <string.h>

typedef struct dMat {
    size_t rows;
    size_t columns;
    double* data;
} dMat;

struct dMat createMatrix(size_t _rows, size_t _columns);

struct dMat makeNullMatrix();

void deleteMatrix(dMat* _in);

void copyMatrix(const dMat* _ori, dMat* _trg);

void addMatrix(const dMat* _lhs, const dMat* _rhs, dMat* _res);

void subtractMatrix(const dMat* _lhs, const dMat* _rhs, dMat* _res);

void addScalar(const dMat* _lhs, double _val, dMat* _res);

void subtractScalar(const dMat* _lhs, double _val, dMat* _res);

void multiplyMatrix(const dMat* _lhs, const dMat* _rhs, dMat* _res, int _method);

#define MATMUL_GENERATE 1
#define MATMUL_COPPERSMITHWINOGRAD 2

void multiplyScalar(const dMat* _lhs, double _val, dMat* _res);

void transposeMatrix(const dMat* _ori, dMat* _trg);

double getMax(const dMat* _mat);

double getMin(const dMat* _mat);

void printMat(const dMat* _ori, unsigned int _totalDigits, unsigned int _point_digits);

#define get(__mat, __row, __col) __mat.data[__row * __mat.columns + __col]

#endif
