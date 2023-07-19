//
// Created by zheng on 2020/1/5.
//

#include "papi_wrap.h"
#include <cstdio>

static unsigned long long seed = 123456789;
unsigned long long rand_longlong()
{
    seed ^= (seed << 21);
    seed ^= (seed >> 35);
    seed ^= (seed << 4);
    return seed;
}

void print_timing_for_python(const std::vector<DataType> &v)
{
    // debug: output timing for python relative performance calculation
    for (const auto &data : v)
    {
        fprintf(stderr, "%llu %llu ", data.elapsed,
                data.timestamp);
    }
    fprintf(stderr, "\n");
}
