//
// Created by eric on 19-3-30.
//

#include <algorithm>
#include "clustering.h"
#define getv(k) (mixed_paths[k].papi[I_PAPI_TOT_INS])

struct CenterType
{
    int l, r;
};

vector<vector<DataType>> calc_classify_fake(vector<DataType> mixed_paths, const double alpha)
{
    vector<vector<DataType>> ret = {mixed_paths};
    return ret;
}

/**
 *
 * @param mixed_paths
 * @param alpha variance radius
 * @return
 */
vector<vector<DataType>> calc_classify(vector<DataType> mixed_paths, const double alpha)
{
    vector<CenterType> centers;
    vector<vector<DataType>> ret;
    sort(mixed_paths.begin(), mixed_paths.end(), [](const DataType &l,const DataType &r) {
        return l.papi[I_PAPI_TOT_INS] < r.papi[I_PAPI_TOT_INS];
    });
    int l = 0, r = 0;
    for (int m = 0; m < mixed_paths.size(); ++m)
    {
        while (r < mixed_paths.size() && getv(r) < getv(m) * (1 + alpha)) ++r;
        while (l< m && getv(l) < getv(m)* (1 - alpha)) ++l;
        if (centers.empty()||(centers.back().r<l))
        {
            centers.push_back(CenterType{l, r});
        } else if (centers.back().r - centers.back().l < r - l)
        {
            centers.pop_back();
            centers.push_back(CenterType{l, r});
        }
    }
    for (const auto &v:centers)
    {
        vector<DataType> group;
        for (auto i = v.l; i < v.r; ++i)
            group.push_back(mixed_paths[i]);
        ret.emplace_back(vector<DataType>(group));
    }

    // push data between clusters
    if (!centers.empty())
    {
        vector<DataType> group;
        for (auto j = 0; j < centers[0].l; ++j)
            group.push_back(mixed_paths[j]);
        ret.emplace_back(vector<DataType>(group));
    }
    for (int i = 0; i < centers.size()-1; ++i)
    {
        vector<DataType> group;
        for (auto j = centers[i].r; j < centers[i + 1].l; ++j)
            group.push_back(mixed_paths[j]);
        ret.emplace_back(vector<DataType>(group));
    }
    return ret;
}

unsigned long long hash_sequence(int n, void *array[])
{
    unsigned long long result = 0;
    for (int i = 0; i < n; ++i)
    {
        auto h = (long long) array[i]; // the "identity hash"
        result ^= h + 0x9E3779B97F4A7C15 + (result << 6u) + (result >> 2u);
    }
    return (unsigned long long)result;
}


vector<vector<DataType>> comm_classify_fake(vector<DataType> &mixed_paths)
{
    return vector<vector<DataType>>{mixed_paths};
}
