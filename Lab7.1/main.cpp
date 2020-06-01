#include <stdio.h>
#include <math.h>
#include <omp.h>
#include <iostream>
#define N 1000

int main(int argc, char** argv)
{
    double b[N];
    double s = 0;
//#pragma omp parallel for
#pragma omp parallel for schedule(static) reduction(+:s)
    for (size_t i = 0; i < N; i++)
    {
        b[i] = i * tan(i * 3.14 / N);
        s += b[i];
    }
//    printf("%f ", s);
    std::cout << s << std::endl;
    return 0;
}