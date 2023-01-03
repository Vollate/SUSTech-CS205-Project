#include "Mat.h"

#include <math.h>
#include <memory.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#ifndef GOTO_MUL
#define GOTO_MUL
#ifdef OPEN_MP
#include <omp.h>
#endif
#ifdef __AVX__
#include <immintrin.h>
#endif

#define max(a, b)                                                                                                                \
    ({                                                                                                                           \
        __typeof__(a) _a = (a);                                                                                                  \
        __typeof__(b) _b = (b);                                                                                                  \
        _a > _b ? _a : _b;                                                                                                       \
    })

#define min(a, b)                                                                                                                \
    ({                                                                                                                           \
        __typeof__(a) _a = (a);                                                                                                  \
        __typeof__(b) _b = (b);                                                                                                  \
        _a < _b ? _a : _b;                                                                                                       \
    })

#ifndef L1CACHE
#define L1CACHE (128 * 1024)
#endif
#ifndef L2CACHE
#define L2CACHE (512 * 1024)
#endif
#define l1_cap_float_size (L1CACHE / 32.0)
#define l2_cap_float_size (L2CACHE / 32.0)

enum FORMAT { RM = 0, CM = 1 };  // row main or column main
enum TRAN { TRAN = 0, NTRAN = 1 };

void pack(const float* ori, float* trg, const size_t ldo, const size_t row_size, const size_t col_size, enum FORMAT f,
          enum TRAN t) {
    switch(f) {
        case RM:
            if(t == NTRAN) {
                for(size_t i = 0; i < row_size; ++i)
                    memcpy(trg + col_size * i, ori + ldo * i, sizeof(float) * col_size);
                return;
            } else if(t == TRAN) {
                float* tmp = trg;
                for(size_t i = 0; i < col_size; ++i)
                    for(size_t j = 0; j < row_size; ++j)
                        *tmp++ = ori[i + col_size * j];
                return;
            } else
                abort();
        case CM:
            if(t == NTRAN) {
                for(size_t i = 0; i < col_size; ++i)
                    memcpy(trg + row_size * i, ori + ldo * i, sizeof(float) * row_size);
                return;
            } else if(t == TRAN) {
                float* tmp = trg;
                for(size_t i = 0; i < row_size; ++i)
                    for(size_t j = 0; j < col_size; ++j)
                        *tmp++ = ori[i + row_size * j];
                return;
            } else
                abort();
        default:
            abort();
    }
}

void unpcak(const float* ori, float* trg, const size_t ldo, const size_t row_size, const size_t col_size, enum FORMAT f,
            enum TRAN t) {
    switch(f) {
        case RM:
            if(t == NTRAN) {
                for(size_t i = 0; i < row_size; ++i)
                    memcpy(trg + ldo * i, ori + col_size * i, sizeof(float) * col_size);
                return;
            } else if(t == TRAN) {
                float* tmp = trg;
                for(size_t i = 0; i < col_size; ++i)
                    for(size_t j = 0; j < row_size; ++j)
                        *tmp++ = ori[i + col_size * j];
                return;
            } else
                abort();
        case CM:
            if(t == NTRAN) {
                for(size_t i = 0; i < col_size; ++i)
                    memcpy(trg + row_size * i, ori + ldo * i, sizeof(float) * row_size);
                return;
            } else if(t == TRAN) {
                float* tmp = trg;
                for(size_t i = 0; i < row_size; ++i)
                    for(size_t j = 0; j < col_size; ++j)
                        *tmp++ = ori[i + row_size * j];
                return;
            } else
                abort();
        default:
            abort();
    }
}

void gemm(const float* A, const float* B, float* C, const size_t M, const size_t N, const size_t K) {

    size_t M_size, N_size, K_size;
    M_size = 10, N_size = 50, K_size = 496;
    size_t M_cur, N_cur, K_cur;
    size_t M_step, N_step, K_step;
    register float* sub_A;

    for(K_cur = 0; K_cur < K; K_cur += K_size) {
        K_step = min(K - K_cur, K_size);

        sub_A = aligned_alloc(32, M * K_step * sizeof(float));
        pack(&A[K_cur], sub_A, K, M, K_step, RM, NTRAN);  // pack A: M*K_step

        for(N_cur = 0; N_cur < N; N_cur += N_size) {
            float *sub_B, *sub_C;
            N_step = min(N - N_cur, N_size);
            sub_B = aligned_alloc(32, N_step * K_step * sizeof(float));
            sub_C = aligned_alloc(32, M * N_step * sizeof(float));
            pack(&B[K_cur * N + N_cur], sub_B, K, N_step, K_step, RM, TRAN);  // pack B: N_step*K_step
            pack(C + N_cur, sub_C, N, M, N_step, RM, NTRAN);                  // pack C: M*N_step

            // calculate

            {
                 #pragma omp parallel for set_threads(8)
                for(size_t m = 0; m < M; ++m)
                    if(!K_step % 8) {
                        float* res = aligned_alloc(32, sizeof(float) * 8);
                        float* tmp_C = &sub_C[m * N_step];
                        __m256 A256, B256, C256;
                        for(int n = 0; n < N_step; ++n) {
                            float* tmp_B = sub_B;
                            A256 = _mm256_load_ps(sub_A);
                            for(int k = 0; k < K_step; ++k) {
                                B256 = _mm256_load_ps(tmp_B);
                                C256 = _mm256_mul_ps(A256, B256);
                                _mm256_store_ps(res, C256);
                                *(tmp_C++) += res[0] + res[1] + res[2] + res[3] + res[4] + res[5] + res[6] + res[7];
                                tmp_B += K_step;
                            }
                        }
                        free(res);
                    } else
                        for(int k = 0; k < K_step; ++k)
                            for(int n = 0; n < N_step; ++n)
                                sub_C[m * N_step + n] += sub_A[m * K_step + k] * sub_B[n * N_step + k];
            }
            unpcak(sub_C, &C[N_cur], N, M, N_step, RM, NTRAN);
            free(sub_B);
            free(sub_C);
        }
        free(sub_A);
    }
}
#undef l1_float_size
#undef l2_float_size
#endif
