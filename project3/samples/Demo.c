#include "Mat.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char** argv) {
    srand(time(NULL));
    dMat m1 = createMatrix(5, 5);
    dMat m2, m3, m4;
    m2 = m3 = m4 = makeNullMatrix();
    double* m1Ptr = m1.data;
    for(size_t i = 0; i != 25; ++i) {
        *m1Ptr = (rand() % 2 ? -1 : 1) * (double)rand() + (double)rand() / RAND_MAX * 2.0 - 1.0;
        ++m1Ptr;
    }
    printf("mat m1:\n");
    printMat(&m1, 20, 5);
    copyMatrix(&m1, &m2);
    transposeMatrix(&m1, &m2);
    double* m2Ptr = m2.data;
    for(size_t i = 0; i != 25; ++i) {
        *m2Ptr = 0.0;
        ++m2Ptr;
    }
    printf("--------------------------\nm2 copy from m1,change m2 doesn\'t affect m1:\nm2:\n");
    printMat(&m2, 20, 5);
    printf("m1:\n");
    printMat(&m1, 20, 5);
    printf("--------------------------\n");
    printf("m1=m1-114514.1919810\n");
    printMat(&m1, 20, 5);
    subtractScalar(&m1, 114514.1919810, &m1);
    printf("--------------------------\n");
    printf("m2=m2+51.4\n");
    addScalar(&m2, 51.4, &m2);
    printMat(&m2, 20, 5);
    printf("--------------------------\n");
    printf("m3=m1-m2\n");
    subtractMatrix(&m1, &m2, &m3);
    printMat(&m3, 20, 5);
    printf("--------------------------\n");
    printf("m4=m3's transpose\n");
    transposeMatrix(&m3, &m4);
    printMat(&m4, 20, 5);
    printf("--------------------------\n");
    printf("m1's max element is %20.5f\nm2's min element is %20.5f\n", getMax(&m1), getMin(&m2));
    deleteMatrix(&m1);
    deleteMatrix(&m2);
    deleteMatrix(&m3);
    deleteMatrix(&m4);
    m1 = createMatrix(4000, 5000);
    m2 = createMatrix(5000, 6000);
    m1Ptr = m1.data, m2Ptr = m2.data;
    for(size_t i = 0; i != 4000 * 5000; ++i) {
        *m1Ptr = (rand() % 2 ? -1 : 1) * (double)rand() + (double)rand() / RAND_MAX * 2.0 - 1.0;
        *m2Ptr = (rand() % 2 ? -1 : 1) * (double)rand() + (double)rand() / RAND_MAX * 2.0 - 1.0;
        ++m1Ptr;
        ++m2Ptr;
    }
    for(size_t i = 4000 * 5000; i != 5000 * 6000; ++i) {
        *m2Ptr = (rand() % 2 ? -1 : 1) * (double)rand() + (double)rand() / RAND_MAX * 2.0 - 1.0;
        ++m2Ptr;
    }
    clock_t start = clock();
    multiplyMatrix(&m1, &m2, &m3, MATMUL_GENERATE);
    printf("--------------------------\n");
    printf("Generate method\'s time cost: %f s\n", (double)(clock() - start) / CLOCKS_PER_SEC);
    start = clock();
    multiplyMatrix(&m1, &m2, &m4, MATMUL_COPPERSMITHWINOGRAD);
    printf("--------------------------\n");
    printf("Coppersmith-Winograd method\'s time cost: %f s\n", (double)(clock() - start) / CLOCKS_PER_SEC);
    printf("\n");

    return 0;
}
