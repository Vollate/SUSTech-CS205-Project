#include "Mat.h"

#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define ErrorLabel(_unused)                                                                                                      \
    MemoryLack:                                                                                                                  \
    printf("Lack of Memory\n");                                                                                                  \
    abort();                                                                                                                     \
    ErrorMatFormat:                                                                                                              \
    printf("Wrong rows/columns\n");                                                                                              \
    abort();

static fMat newMat(size_t rows, size_t columns) {
    if(columns > ULLONG_MAX / rows / sizeof(float))
        goto MemoryLack;
    float* ptr = calloc(rows * columns, sizeof(float));
    if(ptr == NULL)
        goto MemoryLack;
    fMat res = { rows, columns, ptr };
    return res;
    ErrorLabel(newMat)
}

static inline void simGenMatMul(const float* lhs, const float* rhs, float* rePtr, size_t lRows, size_t rColumns, size_t same) {
    for(size_t i = 0; i != lRows; ++i)
        for(size_t j = 0; j != rColumns; ++j) {
            float res = 0.0;
            for(size_t k = 0; k != same; ++k)
                res += lhs[i * same + k] * rhs[k * rColumns + j];
            rePtr[i * rColumns + j] = res;
        }
}

static inline void matMulGenerate(const float* lhs, const float* rhs, float* res, const size_t M, const size_t N, const size_t K,
                                  const size_t strideA, const size_t strideB, const size_t strideC) {
    for(size_t i = 0; i != M; i++) {
        for(size_t j = 0; j != N; j++) {
            float sum = 0.0;
            for(size_t k = 0; k != K; k++) {
                sum += lhs[i * strideA + k] * rhs[k * strideB + j];
            }
            res[i * strideC + j] = sum;
        }
    }
}

static inline void CoppersmithWinografMatMul(float* lhs, float* rhs, float* res, size_t M, size_t N, size_t K, size_t strideA,
                                             size_t strideB, size_t strideC) {
    if((M <= 2) || (M % 2 != 0 || N % 2 != 0 || K % 2 != 0)) {
        return matMulGenerate(lhs, rhs, res, M, N, K, strideA, strideB, strideC);
    }
    float* S1 = (float*)malloc((M / 2) * (K / 2) * sizeof(float));
    float* S2 = (float*)malloc((M / 2) * (K / 2) * sizeof(float));
    float* S3 = (float*)malloc((M / 2) * (K / 2) * sizeof(float));
    float* S4 = (float*)malloc((M / 2) * (K / 2) * sizeof(float));
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

    float* T1 = (float*)malloc((K / 2) * (N / 2) * sizeof(float));
    float* T2 = (float*)malloc((K / 2) * (N / 2) * sizeof(float));
    float* T3 = (float*)malloc((K / 2) * (N / 2) * sizeof(float));
    float* T4 = (float*)malloc((K / 2) * (N / 2) * sizeof(float));
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
    float* M1 = (float*)malloc((M / 2) * (N / 2) * sizeof(float));
    CoppersmithWinografMatMul(lhs, rhs, &M1[0], M / 2, N / 2, K / 2, strideA, strideB, N / 2);
    float* M2 = (float*)malloc((M / 2) * (N / 2) * sizeof(float));
    CoppersmithWinografMatMul(&lhs[K / 2], &rhs[(K / 2) * strideB], &M2[0], M / 2, N / 2, K / 2, strideA, strideB, N / 2);
    float* M3 = (float*)malloc((M / 2) * (N / 2) * sizeof(float));
    CoppersmithWinografMatMul(&S4[0], &rhs[(K / 2) * strideB + (N / 2)], &M3[0], M / 2, N / 2, K / 2, K / 2, strideB, N / 2);
    float* M4 = (float*)malloc((M / 2) * (N / 2) * sizeof(float));
    CoppersmithWinografMatMul(&lhs[(M / 2) * strideA + (K / 2)], &T4[0], &M4[0], M / 2, N / 2, K / 2, strideA, N / 2, N / 2);
    float* M5 = (float*)malloc((M / 2) * (N / 2) * sizeof(float));
    CoppersmithWinografMatMul(&S1[0], &T1[0], &M5[0], M / 2, N / 2, K / 2, K / 2, N / 2, N / 2);
    float* M6 = (float*)malloc((M / 2) * (N / 2) * sizeof(float));
    CoppersmithWinografMatMul(&S2[0], &T2[0], &M6[0], M / 2, N / 2, K / 2, K / 2, N / 2, N / 2);
    float* M7 = (float*)malloc((M / 2) * (N / 2) * sizeof(float));
    CoppersmithWinografMatMul(&S3[0], &T3[0], &M7[0], M / 2, N / 2, K / 2, K / 2, N / 2, N / 2);
    for(size_t i = 0; i != M / 2; i++) {
        for(size_t j = 0; j != N / 2; j++) {
            size_t idx = i * (N / 2) + j;
            res[i * strideC + j] = M1[idx] + M2[idx];
            res[i * strideC + j + (N / 2)] = M1[idx] + M6[idx] + M5[idx] + M3[idx];
            res[(i + (M / 2)) * strideC + j] = M1[idx] + M6[idx] + M7[idx] - M4[idx];
            res[(i + (M / 2)) * strideC + j + (N / 2)] = M1[idx] + M6[idx] + M7[idx] + M5[idx];
        }
    }

#define reset(__ptr)                                                                                                             \
    free(__ptr);                                                                                                                 \
    __ptr = NULL;

    reset(S1) reset(S2) reset(S3) reset(S4) reset(T1) reset(T2) reset(T3) reset(T4) reset(M1) reset(M2) reset(M3) reset(M4)
        reset(M5) reset(M6) reset(M7)
}

void matmul_plain(const fMat* lhs, const fMat* rhs, fMat* res, int method) {
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
            CoppersmithWinografMatMul(lhs->data, rhs->data, res->data, lhs->rows, rhs->columns, lhs->columns, lhs->columns,
                                      rhs->columns, lhs->columns);
            break;
        default:
            printf("Unknown method, use Generate method instead\n");
            simGenMatMul(lhs->data, rhs->data, res->data, lhs->rows, rhs->columns, lhs->columns);
    }
    return;
    ErrorLabel(multiplyMatrix)
}
