//
// Created by zheng on 2020/7/29.
//

#include "pmu.h"
#include "papi_wrap.h"
#include <sys/time.h>
#include <sys/resource.h>
#include <vector>
#include <iostream>

std::vector<long long> get_rusage_data()
{
    static long long values[CNT_RUSAGE_EVENTS];
    static rusage raw_data;
    if (getrusage(RUSAGE_SELF, &raw_data))
    {
        std::cerr << "Error: getrusage failed:"<<strerror(errno);
    }
    auto ret = std::vector<long long>{raw_data.ru_nvcsw - values[0],
                                      raw_data.ru_nivcsw - values[1],
                                      raw_data.ru_majflt - values[2],
                                      raw_data.ru_minflt - values[3]};
    values[0]=raw_data.ru_nvcsw;
    values[1]=raw_data.ru_nivcsw;
    values[2]=raw_data.ru_majflt;
    values[3]=raw_data.ru_minflt;
    return ret;
}
