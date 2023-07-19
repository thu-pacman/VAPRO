//
// Created by eric on 19-7-11.
//

#ifndef SC19_ONLINE_ANALYSE_H
#define SC19_ONLINE_ANALYSE_H
#include "papi_wrap.h"
#include <vector>

extern map<GraphKey, vector<pair<double, double> > > interprocess_info;
std::vector<OnlineCommType>
online_get_comm_data(Graph &graph, int time_l, int time_r);

void online_isend_to_zero(Graph &graph, int time_l, int time_r);

void online_anaylze_init();

bool online_irecv_all();

void online_isend_all(const Graph &calc_graph, const Graph &comm_graph,
                 const Graph &io_graph, int time_l, int time_r);

void online_analyze(ULL current_tsc, ULL last_online_tsc);

void generate_performance_graph(int rank);

#endif //SC19_ONLINE_ANALYSE_H
