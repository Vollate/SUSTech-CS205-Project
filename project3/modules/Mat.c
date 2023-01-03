#include "Mat.h"

#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ErrorLabel(_unused)         \
    MemoryLack:                     \
    printf("Lack of Memory\n");     \
    abort();                        \
    ErrorMatFormat:                 \
    printf("Wrong rows/columns\n"); \
    abort();

static inline dMat newMat(size_t rows, size_t columns) {
    if(columns > ULLONG_MAX / rows / sizeof(double))
        goto MemoryLack;
    double* ptr = calloc(rows * columns, sizeof(double));
    if(ptr == NULL)
        goto MemoryLack;
    dMat res = { rows, columns, ptr };
    return res;
    ErrorLabel(newMat)
}

struct dMat createMatrix(size_t rows, size_t columns) {
    struct dMat res;
    if(rows == 0)
        goto ErrorMatFormat;
    if(columns == 0)
        goto ErrorMatFormat;
    res.data = NULL;
    res = newMat(rows, columns);
    return res;
    ErrorLabel(createMatrix)
}
struct dMat makeNullMatrix() {
    dMat res = { 0, 0, NULL };
    return res;
}

void deleteMatrix(dMat* trg) {
    free(trg->data);
    trg->rows = trg->columns = 0;
    trg->data = NULL;
}

void copyMatrix(const dMat* ori, dMat* trg) {
    if(ori->rows == 0)
        goto ErrorMatFormat;
    if(ori->columns == 0)
        goto ErrorMatFormat;
    if(trg->data != NULL) {
        if(ori->rows != trg->rows)
            goto ErrorMatFormat;
        if(ori->columns != trg->columns)
            goto ErrorMatFormat;
    } else
        *trg = newMat(ori->rows, ori->columns);
    double *oPtr = ori->data, *tPtr = trg->data;
    for(size_t i = 0; i != trg->rows * trg->columns; ++i) {
        *tPtr = *oPtr;
        ++oPtr;
        ++tPtr;
    }
    return;
    ErrorLabel(copyMatrix)
}

static inline void modifyTwoMat(const dMat* lhs, const dMat* rhs, dMat* res, int add) {
    if(lhs->rows == 0 || lhs->columns == 0 || lhs->rows != rhs->rows || lhs->columns != rhs->columns)
        goto ErrorMatFormat;
    if(res->data != NULL) {
        if(lhs->rows != res->rows)
            goto ErrorMatFormat;
        if(lhs->columns != res->columns)
            goto ErrorMatFormat;
    } else {
        *res = newMat(lhs->rows, lhs->columns);
    }
    double *rePtr = res->data, *lPtr = lhs->data, *rPtr = rhs->data;
    if(add)
        for(size_t i = 0; i != res->rows * res->columns; ++i) {
            *rePtr = *lPtr + *rPtr;
            ++rePtr;
            ++lPtr;
            ++rPtr;
        }

    else
        for(size_t i = 0; i != res->rows * res->columns; ++i) {
            *rePtr = *lPtr - *rPtr;
            ++rePtr;
            ++lPtr;
            ++rPtr;
        }
    return;
    ErrorLabel(modifyTwoMat)
}

void addMatrix(const dMat* lhs, const dMat* rhs, dMat* res) {
    modifyTwoMat(lhs, rhs, res, 1);
}

void subtractMatrix(const dMat* lhs, const dMat* rhs, dMat* res) {
    modifyTwoMat(lhs, rhs, res, 0);
}

void addScalar(const dMat* lhs, double val, dMat* res) {
    if(lhs->rows == 0)
        goto ErrorMatFormat;
    if(lhs->rows != lhs->columns)
        goto ErrorMatFormat;
    if(res->data != NULL) {
        if(lhs->rows != res->rows)
            goto ErrorMatFormat;
        if(lhs->columns != res->columns)
            goto ErrorMatFormat;
    } else
        copyMatrix(lhs, res);
    double* rePtr = res->data;
    size_t columns = res->columns;
    for(size_t i = 0; i != columns; ++i)
        rePtr[i * columns + i] += val;
    return;
    ErrorLabel(addScalar)
}

void subtractScalar(const dMat* lhs, double val, dMat* res) {
    if(lhs->rows == 0)
        goto ErrorMatFormat;
    if(lhs->rows != lhs->columns)
        goto ErrorMatFormat;
    if(res->data != NULL) {
        if(lhs->rows != res->rows)
            goto ErrorMatFormat;
        if(lhs->columns != res->columns)
            goto ErrorMatFormat;
    } else
        copyMatrix(lhs, res);
    double* rePtr = res->data;
    size_t columns = res->columns;
    for(size_t i = 0; i != columns; ++i)
        rePtr[i * columns + i] -= val;
    return;
    ErrorLabel(subtractScalar)
}

static inline void simGenMatMul(const double* lhs, const double* rhs, double* rePtr, size_t lrows, size_t rcolumns, size_t same) {
    double res;
    for(size_t i = 0; i != lrows; ++i)
        for(size_t j = 0; j != rcolumns; ++j) {
            res = 0.0;
            for(size_t k = 0; k != same; ++k)
                res += lhs[i * same + k] * rhs[k * rcolumns + j];
            rePtr[i * rcolumns + j] = res;
        }
}

static inline void matMulGenerate(const double* lhs, const double* rhs, double* res, const size_t M, const size_t N,
                                  const size_t K, const size_t strideA, const size_t strideB, const size_t strideC) {
    for(size_t i = 0; i != M; i++) {
        for(size_t j = 0; j != N; j++) {
            double sum = 0.0;
            for(size_t k = 0; k != K; k++) {
                sum += lhs[i * strideA + k] * rhs[k * strideB + j];
            }
            res[i * strideC + j] = sum;
        }
    }
}

static inline void CoppersmithWinogradMatMul(double* lhs, double* rhs, double* res, size_t M, size_t N, size_t K, size_t strideA,
                                             size_t strideB, size_t strideC) {
    if((M <= 2) || (M % 2 != 0 || N % 2 != 0 || K % 2 != 0)) {
        return matMulGenerate(lhs, rhs, res, M, N, K, strideA, strideB, strideC);
    }
    double* S1 = (double*)malloc((M / 2) * (K / 2) * sizeof(double));
    double* S2 = (double*)malloc((M / 2) * (K / 2) * sizeof(double));
    double* S3 = (double*)malloc((M / 2) * (K / 2) * sizeof(double));
    double* S4 = (double*)malloc((M / 2) * (K / 2) * sizeof(double));
    for(size_t i = 0; i != M / 2; i++) {
        for(size_t j = 0; j != K / 2; j++) {
            size_t idxA, offset, idxS = i * (K / 2) + j;
            idxA = (i + (M / 2)) * strideA + j;
            offset = K / 2;
            S1[idxS] = lhs[idxA] + lhs[idxA + offset];
            idxA = i * strideA + j;
            S2[idxS] = S1[idxS] - lhs[idxA];
            offset = (M / 2) * strideA;
            S3[idxS] = lhs[idxA] - lhs[idxA + offset];
            idxA = i * strideA + (K / 2) + j;
            S4[idxS] = lhs[idxA] - S2[idxS];
        }
    }

    double* T1 = (double*)malloc((K / 2) * (N / 2) * sizeof(double));
    double* T2 = (double*)malloc((K / 2) * (N / 2) * sizeof(double));
    double* T3 = (double*)malloc((K / 2) * (N / 2) * sizeof(double));
    double* T4 = (double*)malloc((K / 2) * (N / 2) * sizeof(double));
    {
        for(size_t i = 0; i != K / 2; i++) {
            for(size_t j = 0; j != N / 2; j++) {
                size_t idxB, offset, idxT = i * (N / 2) + j;
                idxB = i * strideB + j;
                offset = (N / 2);
                T1[idxT] = rhs[idxB + offset] - rhs[idxB];
                idxB = (i + (K / 2)) * strideB + (N / 2) + j;
                T2[idxT] = rhs[idxB] - T1[idxT];
                idxB = i * strideB + (N / 2) + j;
                offset = ((K / 2)) * strideB;
                T3[idxT] = rhs[idxB + offset] - rhs[idxB];
                idxB = (i + (K / 2)) * strideB + j;
                T4[idxT] = T2[idxT] - rhs[idxB];
            }
        }
    }
    double* M1 = (double*)malloc((M / 2) * (N / 2) * sizeof(double));
    CoppersmithWinogradMatMul(lhs, rhs, &M1[0], M / 2, N / 2, K / 2, strideA, strideB, N / 2);
    double* M2 = (double*)malloc((M / 2) * (N / 2) * sizeof(double));
    CoppersmithWinogradMatMul(&lhs[K / 2], &rhs[(K / 2) * strideB], &M2[0], M / 2, N / 2, K / 2, strideA, strideB, N / 2);
    double* M3 = (double*)malloc((M / 2) * (N / 2) * sizeof(double));
    CoppersmithWinogradMatMul(&S4[0], &rhs[(K / 2) * strideB + (N / 2)], &M3[0], M / 2, N / 2, K / 2, K / 2, strideB, N / 2);
    double* M4 = (double*)malloc((M / 2) * (N / 2) * sizeof(double));
    CoppersmithWinogradMatMul(&lhs[(M / 2) * strideA + (K / 2)], &T4[0], &M4[0], M / 2, N / 2, K / 2, strideA, N / 2, N / 2);
    double* M5 = (double*)malloc((M / 2) * (N / 2) * sizeof(double));
    CoppersmithWinogradMatMul(&S1[0], &T1[0], &M5[0], M / 2, N / 2, K / 2, K / 2, N / 2, N / 2);
    double* M6 = (double*)malloc((M / 2) * (N / 2) * sizeof(double));
    CoppersmithWinogradMatMul(&S2[0], &T2[0], &M6[0], M / 2, N / 2, K / 2, K / 2, N / 2, N / 2);
    double* M7 = (double*)malloc((M / 2) * (N / 2) * sizeof(double));
    CoppersmithWinogradMatMul(&S3[0], &T3[0], &M7[0], M / 2, N / 2, K / 2, K / 2, N / 2, N / 2);
    for(size_t i = 0; i != M / 2; i++) {
        for(size_t j = 0; j != N / 2; j++) {
            size_t idx = i * (N / 2) + j;
            res[i * strideC + j] = M1[idx] + M2[idx];
            res[i * strideC + j + (N / 2)] = M1[idx] + M6[idx] + M5[idx] + M3[idx];
            res[(i + (M / 2)) * strideC + j] = M1[idx] + M6[idx] + M7[idx] - M4[idx];
            res[(i + (M / 2)) * strideC + j + (N / 2)] = M1[idx] + M6[idx] + M7[idx] + M5[idx];
        }
    }

#define reset(__ptr) \
    free(__ptr);     \
    __ptr = NULL;

    reset(S1) reset(S2) reset(S3) reset(S4) reset(T1) reset(T2) reset(T3) reset(T4) reset(M1) reset(M2) reset(M3) reset(M4)
        reset(M5) reset(M6) reset(M7)
}

void multiplyMatrix(const dMat* lhs, const dMat* rhs, dMat* res, int method) {
    if(lhs->rows == 0 || lhs->columns == 0 || rhs->columns == 0)
        goto ErrorMatFormat;
    if(lhs->columns != rhs->rows)
        goto ErrorMatFormat;
    if(res->data != NULL) {
        if(res->rows != lhs->rows)
            goto ErrorMatFormat;
        if(res->columns != rhs->columns)
            goto ErrorMatFormat;
    } else
        *res = newMat(lhs->rows, rhs->columns);
    switch(method) {
        case MATMUL_GENERATE:
            simGenMatMul(lhs->data, rhs->data, res->data, lhs->rows, rhs->columns, lhs->columns);
            break;
        case MATMUL_COPPERSMITHWINOGRAD:
            CoppersmithWinogradMatMul(lhs->data, rhs->data, res->data, lhs->rows, rhs->columns, lhs->columns, lhs->columns,
                                      rhs->columns, lhs->columns);
            break;
        default:
            printf("Unkonw method, use Generate method instead\n");
            simGenMatMul(lhs->data, rhs->data, res->data, lhs->rows, rhs->columns, lhs->columns);
    }
    return;
    ErrorLabel(multiplyMatrix)
}

void multiplyScalar(const dMat* lhs, double val, dMat* res) {
    if(lhs->rows == 0)
        goto ErrorMatFormat;
    if(lhs->columns == 0)
        goto ErrorMatFormat;
    if(res->data != NULL) {
        if(lhs->rows == res->rows)
            goto ErrorMatFormat;
        if(lhs->columns == res->columns)
            goto ErrorMatFormat;
    } else
        *res = newMat(lhs->rows, lhs->columns);
    double *lPtr = lhs->data, *rePtr = res->data;
    for(size_t i = 0; i != lhs->rows * lhs->columns; ++i) {
        *rePtr = *lPtr * val;
        ++lPtr;
        ++rePtr;
    }
    return;
    ErrorLabel(multiplyScalar)
}

void transposeMatrix(const dMat* ori, dMat* trg) {
    if(trg->data != NULL) {
        if(ori->rows != trg->columns || ori->columns != trg->rows)
            goto ErrorMatFormat;
    } else
        *trg = newMat(ori->columns, ori->rows);
    double *oPtr = ori->data, *tPtr = trg->data;
    const size_t rows = ori->rows, columns = ori->columns;
    if(ori->columns > ori->rows)
        for(size_t i = 0; i != ori->rows; ++i)
            for(size_t j = 0; j != ori->columns; ++j)
                tPtr[j * rows + i] = oPtr[i * columns + j];
    else
        for(size_t j = 0; j != ori->columns; ++j)
            for(size_t i = 0; i != ori->rows; ++i)
                tPtr[j * rows + i] = oPtr[i * columns + j];
    return;
    ErrorLabel(transposeMatrix)
}

double getMax(const dMat* mat) {
    double res = mat->data[0];
    double* ptr = mat->data;
    for(size_t i = 1; i != mat->rows * mat->columns; ++i) {
        res = fmax(res, *ptr);
        ++ptr;
    }
    return res;
}

double getMin(const dMat* mat) {
    double res = mat->data[0];
    double* ptr = mat->data;
    for(size_t i = 1; i != mat->rows * mat->columns; ++i) {
        res = fmin(res, *ptr);
        ++ptr;
    }
    return res;
}

void printMat(const dMat* ori, unsigned int totalDigits, unsigned int pointDigits) {
    if(pointDigits > 99 || totalDigits < pointDigits) {
        printf("Invalid digits\n");
        abort();
    }
    int length = snprintf(NULL, 0, "%u", totalDigits);
    char* mid = malloc(length + 2);
    snprintf(mid, length + 2, "%u", totalDigits);
    mid[length] = '.';
    mid[length + 1] = '\0';
    char format[10] = "%";
    strcat(format, mid);
    free(mid);
    length = snprintf(NULL, 0, "%u", pointDigits);
    mid = malloc(length + 1);
    snprintf(mid, length + 1, "%u", pointDigits);
    strcat(format, mid);
    strcat(format, "f ");
    free(mid);
    double* oPtr = ori->data;
    size_t columns = ori->columns;
    for(size_t i = 0, j = 1; i != ori->rows * ori->columns; ++i, ++j) {
        printf(format, *oPtr);
        if(j == columns) {
            printf("\n");
            j = 0;
        }
        ++oPtr;
    }
    return;
    ErrorLabel(printMat)
}
