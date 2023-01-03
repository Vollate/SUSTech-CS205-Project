#include "Mat.h"

#include <limits.h>

int main(int argc, char** argv) {
    dMat m1, m2, m3, m4;
    m3 = m4 = makeNullMatrix();

    //m1=createMatrix(ULLONG_MAX,ULLONG_MAX);
    
    m1 = createMatrix(12, 32);

    // m2=createMatrix(0,0);
    // addScalar(&m1,1.2,&m1);
    // multiplyScalar(&m1,3,&m3);
    
    m2=createMatrix(1,3);

    //addMatrix(&m2,&m1,&m3);
    //subtractMatrix(&m2,&m1,&m4);

    m4 = createMatrix(20, 1);
    deleteMatrix(&m1);

    // multiplyMatrix(&m4,&m3,&m1,MATMUL_COPPERSMITHWINOGRAD);
    // printMat(&m2, 1,43);

    return 0;
}
