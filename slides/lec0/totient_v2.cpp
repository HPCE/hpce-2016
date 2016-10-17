#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <atomic>

#include "gcd.hpp"

#include "tbb/parallel_for.h"

uint64_t totient_v2(uint64_t n)
{
    std::atomic<uint64_t> count;

    //for(uint64_t i=1; i<=n; i++){
    tbb::parallel_for(uint64_t(1), (n+1), [&](uint64_t i){
        if(gcd(i,n)==1){
            count += 1;
        }
    });

    return count;
}

int main(int argc, char *argv[])
{
    double exponent=strtod(argv[1], 0);
    uint64_t n=std::exp(exponent);

    auto startTime=std::chrono::high_resolution_clock::now();

    uint64_t count=totient_v2(n);

    auto endTime=std::chrono::high_resolution_clock::now();

    double time=std::chrono::duration<double>(endTime-startTime).count();

    fprintf(stdout, "e^%1.3f, %10llu, %10llu, %9.6f\n", exponent, n, count, time);

    return 0;
}
