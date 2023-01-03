#include "GOTO_mul.c"
#include "Mat.h"

#include "limits.h"
#include "stdio.h"

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

void matmul_improved(const fMat* lhs, const fMat* rhs, fMat* res) {
    if(lhs->rows == 0 || lhs->columns == 0 || rhs->columns == 0)
        goto ErrorMatFormat;
    if(lhs->columns != rhs->rows)
        goto ErrorMatFormat;
    if(lhs == res || rhs == res)
        goto ErrorMatFormat;
    if(res->data != NULL) {
        if(res->rows != lhs->rows)
            goto ErrorMatFormat;
        if(res->columns != rhs->columns)
            goto ErrorMatFormat;
    } else
        *res = newMat(lhs->rows, rhs->columns);
    gemm(lhs->data,rhs->data,res->data,lhs->rows,rhs->columns,lhs->rows);
    return;
    ErrorLabel(matmul_improved)
}

#undef ErrorLabel
