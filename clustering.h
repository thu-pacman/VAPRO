//
// Created by eric on 19-3-30.
//

#ifndef SC19_CLUSTERING_H
#define SC19_CLUSTERING_H

#include "papi_wrap.h"
#include <vector>
using namespace std;

vector<vector<DataType>> calc_classify(vector<DataType> mixed_paths, const double alpha);
vector<vector<DataType>> calc_classify_fake(vector<DataType> mixed_paths, const double alpha);

vector<vector<DataType>> comm_classify_fake(vector<DataType> &mixed_paths);

unsigned long long hash_sequence(int n, void *array[]);

#endif //SC19_CLUSTERING_H
