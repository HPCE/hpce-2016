#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "gcd.hpp"

uint64_t totient_v0(uint64_t n)
{
    uint64_t count=0;

    for(uint64_t i=1; i<=n; i++){
        if(gcd(i,n)==1){
            count = count + 1;
        }
    }

    return count;
}

int main(int argc, char *argv[])
{
    double exponent=strtod(argv[1], 0);
    uint64_t n=std::exp(exponent);

    auto startTime=std::chrono::high_resolution_clock::now();

    uint64_t count=totient_v0(n);

    auto endTime=std::chrono::high_resolution_clock::now();

    double time=std::chrono::duration<double>(endTime-startTime).count();

    fprintf(stdout, "e^%1.3f, %10llu, %10llu, %9.6f\n", exponent, n, count, time);

    return 0;
}
