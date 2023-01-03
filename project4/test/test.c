#include <Mat.h>

#include <cblas.h>
#include <float.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define new_line printf("==========================================================\n")

int main(int argc, char** argv) {
#ifndef TEST_DEBUG
    if(argc != 3) {
        printf("illegal parameter\n");
        abort();
    }
#endif
    size_t rows, columns;
    rows = columns = 0;
    char* cPtr = argv[1];
    while(*cPtr != '\0')
        rows = 10 * rows + (*(cPtr++) - '0');
    cPtr = argv[2];
    while(*cPtr != '\0')
        columns = 10 * columns + (*(cPtr++) - '0');
    fMat lhs = createMatrix(rows, columns), rhs = createMatrix(columns, rows);
    float *lPtr = lhs.data, *rPtr = rhs.data;
    for(size_t i = 0; i < rows * columns; ++i) {
                lPtr[i] = /*i * rand() % 191+*/ (float)(rand() % 741) / 1000;
                rPtr[i] = /*i * rand() % 114 +*/ (float)(rand() % 874) / 1000;
//        *(lPtr++) = *(rPtr++) = 2;
    }
    fMat resImproved, resPlain, resBlas;
    resImproved.rows = resImproved.columns = resPlain.rows = resPlain.columns = rows;
    resPlain.data = calloc(rows * rows, sizeof(float));
    resImproved.data = calloc(rows * rows, sizeof(float));
    resBlas.data = calloc(rows * rows, sizeof(float));
    struct timespec start, finish;
    float elapsed;
    clock_gettime(CLOCK_MONOTONIC, &start);
    matmul_plain(&lhs, &rhs, &resPlain, MATMUL_GENERATE);
    clock_gettime(CLOCK_MONOTONIC, &finish);
    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    new_line;
    printf("Plain matrix multiply time cost %f s\n", elapsed);
    clock_gettime(CLOCK_MONOTONIC, &start);
    matmul_improved(&lhs, &rhs, &resImproved);
//    matmul_plain(&lhs, &rhs, &resPlain, MATM UL_COPPERSMITHWINOGRAD);
    clock_gettime(CLOCK_MONOTONIC, &finish);
    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    new_line;
    printf("Improved matrix multiply time cost %f s\n", elapsed);
    clock_gettime(CLOCK_MONOTONIC, &start);
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, rows, rows, columns, 1, lhs.data, columns, rhs.data, rows, 1,
                resBlas.data, rows);
    clock_gettime(CLOCK_MONOTONIC, &finish);
    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    new_line;
    printf("OpenBlas matrix multiply time cost %f s\n", elapsed);
    new_line;
    //        float *impPtr = resImproved.data, *blasPtr = resBlas.data;
    //        {
    //     #pragma omp parallel for
    //            for(size_t i = 0; i < resImproved.rows * resImproved.columns; ++i)
    //                if(impPtr[i] - blasPtr[i] > 0.1 || impPtr[i] - blasPtr[i] < -0.1) {
    //                    printf("at %lu\nimproved: %.6f\nblas: %.6f\n", i, impPtr[i], blasPtr[i]);
    //                    new_line;
    ////                    abort();
    //                }
    //        }
    return 0;
}
