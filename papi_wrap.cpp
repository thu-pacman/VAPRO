#include <mpi.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <ctime>
#include <zconf.h>
#include <cstdlib>
#include <cstdio>
#include <climits>
#include <papi.h>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <string>
#include <cassert>
#include <algorithm>
#include <execinfo.h> // gcc-version backtrace
#include <json/json.h>
//#define UNW_LOCAL_ONLY
//#include <libunwind.h> // libunwind-version backtrace
#include "papi_wrap.h"
#include "clustering.h"

// gettimeofday
//#include<sys/time.h>
#include<unistd.h>
#include <sstream>

using namespace std;
#ifndef CPU_FREQ
#warning Use default TSC frequency
#define CPU_FREQ 2.3e9
#endif
const long long Cpu_freq = CPU_FREQ;

/* Get actual CPU time in seconds */
float gettime()
{
    return((float)PAPI_get_virt_usec()*1000000.0);
}

//using DataType=std::vector<long long>;
//using GraphKey=std::pair<void*,void*>;
//using GraphValue=std::vector<DataType >;
int events[5] = {PAPI_TOT_INS, PAPI_TOT_CYC}; //, PAPI_SR_INS}; //, PAPI_LD_INS, PAPI_L3_TCA};

const int J_MPI_INIT = 0;
const int J_MPI_BARRIER = 23;
const int J_MPI_WAIT = 317;
const int J_MPI_COMM_RANK = 61;
const int J_MPI_COMM_SIZE = 68;
const int J_MPI_WTIME = 359;
const int J_MPI_WAITANY = 319;
const int J_MPI_WAITALL = 318;
const set<int> FILTER_OUT_MPI = {J_MPI_INIT, J_MPI_BARRIER, J_MPI_WAIT, J_MPI_COMM_RANK, J_MPI_COMM_SIZE, J_MPI_WTIME,
                                 J_MPI_WAITANY, J_MPI_WAITALL};
//const set<int> FILTER_OUT_MPI = {J_MPI_INIT, J_MPI_BARRIER, J_MPI_WAIT, J_MPI_COMM_RANK, J_MPI_COMM_SIZE, J_MPI_WTIME};

struct SummaryType
{
    double total_run_time;
    double covered_calc_time, covered_comm_time;
    int edge_cnt, vertex_cnt;

    /**
     *
     * @return String of a json with line feed
     */
    string generate_summary_json()
    {
        Json::Value output;
        ostringstream os;
        output["total_run_time"]=total_run_time;
        output["covered_calc_time"]=covered_calc_time;
        output["covered_comm_time"]=covered_comm_time;
        output["calc_covered_ratio"]=covered_calc_time/total_run_time;
        output["comm_covered_ratio"]=covered_comm_time/total_run_time;
        output["total_covered_ratio"] = (covered_calc_time + covered_comm_time) / total_run_time;
        output["edge_cnt"] = edge_cnt;
        output["vertex_cnt"] = vertex_cnt;

        Json::FastWriter fast_writer;
        os << fast_writer.write(output);
        return os.str();
    }
}summary;

//DataType accumulate_data;
typedef std::pair<void*,void*> GraphKey;
typedef std::vector<DataType > GraphValue;


static void *last_call_addr = nullptr;
int cnt_address;
map<void*, int> address_map;
map<void*, int> address_to_func;
map<void *, DataType> last_data_map;
typedef map<GraphKey, GraphValue> Graph;
Graph graph_comm, graph_calc;
// counter of clustering result
map<GraphKey, int> cluster_cnt;
bool init_flag = false;
// finish flag is necessary because of the occurrence of hooked function in print_graph
bool finish_flag = false;
static int mpi_rank = -1;
VertexType vertex_type = VertexType::CallSite;


// high-performance clock
static __inline__ unsigned long long rdtsc(void)
{
    unsigned hi, lo;
    __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
    return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
    //------------------------------------------
    // gettimeofday function
	//struct  timeval end;
	//unsigned  long diff;
	//gettimeofday(&end,NULL);
	//diff = 1000000 * (end.tv_sec)+ end.tv_usec;
	//return diff;
}

unsigned long long init_time;
unsigned long long last_time;

void papi_start_counters()
{
    int ret;
    if ((ret = PAPI_start_counters(events, CNT_EVENTS)) != PAPI_OK) {
        fprintf(stderr, "PAPI failed to start counters: %s\n", PAPI_strerror(ret));
        exit(1);
    }
}

void papi_init()
{
    int ret;
    init_time = rdtsc();
    if (PAPI_num_counters() < CNT_EVENTS) {
        fprintf(stderr, "No hardware counters here, or PAPI not supported.\n");
        exit(1);
    }
    //if ((ret = PAPI_set_domain(PAPI_DOM_ALL)) != PAPI_OK)
    if ((ret = PAPI_set_domain(PAPI_DOM_USER | PAPI_DOM_KERNEL)) != PAPI_OK)
    //if ((ret = PAPI_set_domain(PAPI_DOM_USER)) != PAPI_OK)
    {
        fprintf(stderr, "PAPI failed to set_domain: %s\n", PAPI_strerror(ret));
        exit(1);
    }

    // get VERTEX_GRAIN
    char *vertex_path_str = getenv("VERTEX_GRAIN");
    int vertex_path_int = (vertex_path_str == 0) ? VertexType::CallSite : atoi(vertex_path_str);
//        if (mpi_rank == 0)
//    fprintf(stderr, "VERTEX_GRAIN=%d\n", vertex_path_int);
    assert(!(vertex_path_int < 0 || vertex_path_int > 2));
    if (vertex_path_int < 0 || vertex_path_int > 2)
    {
        fprintf(stderr, "vertex path environment variable error");
        exit(-1);
    } else
        vertex_type = (VertexType) vertex_path_int;

    init_flag = true;
    papi_start_counters();
}

DataType papi_get_data()
{
    // debug
    int ret;
    long long values[CNT_EVENTS];
    if ((ret = PAPI_read_counters(values, CNT_EVENTS)) != PAPI_OK) {
        fprintf(stderr, "PAPI failed to read counters: %s\n", PAPI_strerror(ret));
        exit(1);
    }
	unsigned long long cur_time=rdtsc();
    DataType retv;
    retv.set_papi_data(values);
    retv.elapsed = cur_time - last_time; // append wall time to locate this record
    retv.timestamp = cur_time;
	last_time=cur_time;
    return retv;
}

void insert_data(map<GraphKey, GraphValue> &graph, void *last_addr, void *current_addr, DataType& papi_data)
{
    if (!address_map.count(current_addr))
    {
        address_map[current_addr] = cnt_address++;
        address_to_func[current_addr] = papi_data.mpi_func;
    }
    if (!address_map.count(last_addr))
    {
        address_map[last_addr] = cnt_address++;
    }
    auto key = make_pair(last_addr, current_addr);

    if (!graph.count(key))
        graph[key]=GraphValue();
    graph[key].push_back(papi_data);

//    // differentiate to calculate self loop data
//    for (int i = 0; i < CNT_EVENTS+1; ++i)
//    {
//        accumulate_data[i] += papi_data[i];
//    }
//
//    // insert self loop value
//    if (last_addr != current_addr && last_data_map.count(current_addr))
//    {
//        DataType self_loop_data{accumulate_data};
//        for (int i = 0; i < CNT_EVENTS+1; ++i)
//            self_loop_data[i] -= last_data_map[current_addr][i];
//        key = make_pair(current_addr, current_addr);
//        if (!graph.count(key))
//            graph[key] = GraphValue();
//        self_loop_data.insert(self_loop_data.end(), mpi_data.begin(), mpi_data.end());
//        if (self_loop_data.size()<all_data.size())
//            self_loop_data.resize(all_data.size());
//        graph[key].push_back(self_loop_data);
//    }
//
//    // update last call data for the current call point
//    last_data_map[current_addr] = accumulate_data;
}

void *backtrace_buffer[10];
int trace_size;

__attribute_noinline__ void* get_invoke_point()
{
    //This compiler function(MACRO?) have some strange problems
    //return __builtin_return_address(2);
//    constexpr int size = 5;
    if (vertex_type == VertexType::CallSite)
    {
//        trace_size = unw_backtrace(buffer, 5);
//        return buffer[4];
        for (int i = 0; i < trace_size; ++i)
        {
            if ((unsigned long long) backtrace_buffer[i] < 0x00645cbdeb8eLL)
            {
                //output traceback
                //int rank;
                //PMPI_Comm_rank(MPI_COMM_WORLD, &rank);
                //if (rank==0)
                //{
                //    puts("=======================");
                //    backtrace_symbols_fd(buffer,i+1,1);
                //}

                //char **symbuffer;
                //symbuffer=backtrace_symbols(buffer,size);
                //printf("%s\n",symbuffer[i]);
                //free(symbuffer);
//                if (mpi_rank == 0 && i > 4)
//                if (i > 4)
//                {
//                    for (int j = 0; j < trace_size; ++j)
//                    {
//                        printf("d%d=%lx ch=%d\n", j, (unsigned long) buffer[j],
//                               ((unsigned long long) buffer[j] < 0x00645cbdeb8eLL));
//                    }
//                    {
//                        puts("=====");
//                        backtrace_symbols_fd(buffer,i+1,1);
//                    }
//                    puts("=======================\n");
//                }
                return backtrace_buffer[i];
            }
        }
    } else if (vertex_type == VertexType::CallPath)
    {
//        trace_size = unw_backtrace(buffer, 10);
//        // debug
//        if (mpi_rank==0)
//        {
//            char **symbuffer;
//            for (int i=0;i<trace_size;++i)
//            {
//                symbuffer = backtrace_symbols(buffer, size);
//                printf("%s\n", symbuffer[i]);
//                free(symbuffer);
//            }
//            puts("----====----====----====----====----====----");
//        }
        return (void *) hash_sequence(trace_size, backtrace_buffer);
    }

//    printf("==ERROR===\n");
//    for (int j = 0; j < trace_size; ++j)
//    {
//        printf("d%d=%lx ch=%d\n", j, (unsigned long) buffer[j],
//               ((unsigned long long) buffer[j] < 0x00645cbdeb8eLL));
//    }
//    printf("=====\n");

    fprintf(stderr, "Backtrace Error\n");
    exit(0);
}

/**
 *
 * @param suffix should be 0 or 1. 0 is before MPI invocation and 1 is after.
 */
//__attribute_noinline__ void papi_update(int suffix, int function_number, int count)
__attribute_noinline__ void papi_update(int suffix, int mpi_func, int count, int target, void *mpi_comm)
{
    // if it has not been inited
    if (!init_flag)
    {
        papi_init();
    }
    // if finished
    if (finish_flag)
        return;
//    static long long updata_cnt = 0;
//    updata_cnt++;
//    if (updata_cnt % 1000000 == 0)
//    {
//        printf("update_cnt=%lld\n", updata_cnt);
//    }

    long long real_addr;
    if (suffix == 1 && last_call_addr)
    {
        real_addr = (long long) last_call_addr;
    } else {
        real_addr = (vertex_type == VertexType::Function) ?
                    mpi_func : (long long) get_invoke_point();
    }
//    void *current_call_addr = (void *) ((real_addr << 1) | (suffix&1));
    void *current_call_addr = (void *) (real_addr);
    PMPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    // debug
//    if (rank==0)
//    {
//        //printf("%llx papi_update\n",(long long)real_addr);//debug
//        //printf("%llx papi_update direct\n",(long long)__builtin_return_address(3));//debug
//    }
    auto papi_data = papi_get_data();
    papi_data.mpi_func = mpi_func;
    papi_data.mpi_count = count;
    papi_data.target = target;
    papi_data.mpi_comm = mpi_comm;
    if (!(suffix == 1 && FILTER_OUT_MPI.count(mpi_func)))
    {
        insert_data((suffix == 0) ? graph_calc : graph_comm, last_call_addr, current_call_addr, papi_data);
    }
    last_call_addr = current_call_addr;

    // deubg
    if (rdtsc() - init_time > Cpu_freq * 1800)
    {
        print_graph(mpi_rank);
        finish_flag = true;
    }

    papi_get_data(); // reset PAPI counter
}

void *get_addr_from_mixture(void * p)
{
    return (void *) (((long) p) >> 1);
}

int get_status_from_mixture(void * p)
{
    return (int) (((long) p) & 1);
}

struct Interval
{
    double l, r;
    double perf;
};
typedef vector<Interval> PathPerformance;

PathPerformance calc_path(GraphValue &path_data)
{
    unsigned long long min_elapsed=ULONG_LONG_MAX;
    PathPerformance ret;
    Interval interval;
    for (const auto &value:path_data)
        min_elapsed = (min_elapsed <= value.elapsed) ? min_elapsed : value.elapsed;
    for (const auto &value:path_data)
    {
        interval.perf = min_elapsed / double(value.elapsed);
        interval.l = double(value.timestamp - value.elapsed - init_time) / CPU_FREQ;
        interval.r = double(value.timestamp - init_time) / CPU_FREQ;
//        // debug
//        if (interval.l<1&& interval.r>1)
//        {
//            for (auto t:path_data)
//                printf("Warn: %d %d %lf %lf %d %d\n", t.mpi_func, t.mpi_count, interval.l, interval.r, t.src, t.dst);
//        }
        ret.push_back(interval);
    }
    return ret;
}

bool path_data_adequate(const GraphValue &path_data)
{
    return path_data.size()>=5;
}

bool path_stable(const GraphValue &path_data)
{
    //debug
//    return true;

    const double variance = 0.04;
    long long inst_min = LONG_LONG_MAX, inst_max = 0;

    for_each(path_data.begin(),path_data.end(),
            [&inst_min,&inst_max](const DataType& data)
            {
                inst_min = min(inst_min, data.papi[I_PAPI_TOT_INS]);
                inst_max = max(inst_max, data.papi[I_PAPI_TOT_INS]);
            });
    // debug
//    printf("unstable=%lld %lld\n", inst_max, inst_min);
    return (inst_max - inst_min) / (double(inst_max + inst_min) / 2) < variance;
}

vector<double> merge_interval_arithmetic_mean(vector<Interval> &intervals, int l, int r)
{
    sort(intervals.begin(),intervals.end(), [](Interval a, Interval b){ return a.l < b.l; });
    // debug
//    printf("intervals len=%lu\n", intervals.size());
//    for_each(intervals.begin(), intervals.end(), [](Interval interval) {
//        printf("%lf %lf %lf\n", interval.l, interval.r, interval.perf);
//    });
    double sum = 0;
    int cnt = 0;
    auto rit = intervals.cbegin();
    vector<double> ret;
    vector<int> rank_r;
    int lit = 0;
    for (int i = 0; i < intervals.size(); ++i)
        rank_r.push_back(i);
    sort(ret.begin(), ret.end(), [&intervals](int a, int b) { return intervals[a].r < intervals[b].r; });

    for (int i = l; i < r; ++i)
    {
//        // debug
//        printf("=================i=%d========\n", i);
        // rit is iterator of the interval whose l is the largest
        while (rit != intervals.cend() && rit->l <= i + 1)
        {
            ++cnt;
            sum += rit->perf;
            if (i == 0)
                printf("+ %lf %lf\n", rit->l, rit->r);
            ++rit;
        }
        while (intervals[rank_r[lit]].r < i)
        {
            --cnt;
            sum -= intervals[rank_r[lit]].perf;
            if (i == 1)
                printf("- %lf %lf\n", intervals[rank_r[lit]].l, intervals[rank_r[lit]].r);
            ++lit;
        }
        printf("i=%2d cnt=%d\n", i, cnt);
        if (cnt)
            ret.push_back(sum / cnt);
        else // -1 means this interval is not covered
            ret.push_back(-0.00001);
    }
    return ret;
}

vector<double> merge_interval_weighted_mean(vector<Interval> &intervals, int l, int r, double step = 1)
{
    sort(intervals.begin(), intervals.end(), [](Interval a, Interval b) { return a.l < b.l; });
    // debug
//    printf("intervals len=%lu\n", intervals.size());
//    for_each(intervals.begin(), intervals.end(), [](Interval interval) {
//        printf("%lf %lf %lf\n", interval.l, interval.r, interval.perf);
//    });
    double sum = 0, partial_sum = 0, partial_cnt = 0;
    int cnt = 0;
    auto rit = intervals.cbegin();
    vector<double> ret;
    vector<int> rank_r;
    int lit = 0;
    for (int i = 0; i < intervals.size(); ++i)
        rank_r.push_back(i);
    sort(ret.begin(), ret.end(), [&intervals](int a, int b) { return intervals[a].r < intervals[b].r; });

    for (double i = l; i < r; i += step)
    {
//        // debug
//        printf("=================i=%d========\n", i);
        partial_cnt = partial_sum = 0;
        // rit is iterator of the interval whose l is the largest
        while (rit != intervals.cend() && rit->l <= i + step)
        {
            ++cnt;
            sum += rit->perf;
            partial_sum -= rit->perf * (rit->l - i);
            partial_cnt -= (rit->l - i);
//            if (i == 0)
//                printf("+ %lf %lf\n", rit->l, rit->r);
            ++rit;
        }
        while (lit < intervals.size() && intervals[rank_r[lit]].r < i + step)
        {
            --cnt;
            sum -= intervals[rank_r[lit]].perf;
            partial_sum += intervals[rank_r[lit]].perf * (intervals[rank_r[lit]].r - i);
            partial_cnt += (intervals[rank_r[lit]].r - i);
//            if (i == 1)
//                printf("- %lf %lf\n", intervals[rank_r[lit]].l, intervals[rank_r[lit]].r);
            ++lit;
        }
//        printf("i=%2d cnt=%d\n", i, cnt);
        if (fabs(cnt * step + partial_cnt) >= eps)
        {
            ret.push_back((sum * step + partial_sum) / (cnt * step + partial_cnt));
        } else
        { // -1 means this interval is not covered
            ret.push_back(-0.005);
        }
    }
    return ret;
}

vector<double> calc_performance_process(Graph &graph, int time_l, int time_r)
{
    vector<Interval> all_intervals;
    for (auto &kv : graph)
    {
        vector<vector<DataType>> classified = calc_classify(kv.second, 0.02);
        int cnt_stable = 0, cnt_total = 0;
        for (auto &v:classified)
        {
            if (path_data_adequate(v) && path_stable(v))
            {
                cnt_stable++;
                auto new_intervals=calc_path(v);
                // statistical data
                for (auto &vv: new_intervals)
                {
                    summary.covered_calc_time += vv.r - vv.l;
                }
                all_intervals.insert(all_intervals.end(), new_intervals.begin(), new_intervals.end());
            }
            cnt_total++;
        }
        // to output clustering counter
        cluster_cnt[kv.first] = cnt_stable;
    }
//    printf("all_interval.size=%lu stable=%d total=%d\n", all_intervals.size(), cnt_stable, cnt_total);
    auto ret = merge_interval_weighted_mean(all_intervals, time_l, time_r);
    return ret;
}

vector<vector<DataType>> comm_classify(vector<DataType> &mixed_paths)
{
    map<Comm_key, vector<DataType>> classified_paths;
    for (const auto &data:mixed_paths)
    {
        const Comm_key key = data.to_comm_key();
        if (classified_paths.count(key) == 0)
        {
            classified_paths[key] = vector<DataType>{};
        }
        classified_paths[key].push_back(data);
    }
    vector<vector<DataType>> ret;
    for (const auto &kv:classified_paths)
        ret.push_back(kv.second);
    return ret;
}

vector<double> comm_performance_process(Graph &graph, int time_l, int time_r)
{
    vector<Interval> all_intervals;
    for (auto &kv : graph)
    {
        vector<vector<DataType>> classified = comm_classify(kv.second);
        // debug
        unsigned long newlen=0;

        int cnt_stable = 0, cnt_total = 0;
        for (auto &v:classified)
        {
            if (path_data_adequate(v))
            {
                cnt_stable++;
                auto new_intervals = calc_path(v);
                // statistical data
                for (auto &vv: new_intervals)
                {
                    summary.covered_comm_time += vv.r - vv.l;
                    newlen++;
                }
                all_intervals.insert(all_intervals.end(), new_intervals.begin(), new_intervals.end());
            }
            cnt_total++;
        }
        assert(kv.second.size()>=newlen);
        cluster_cnt[kv.first] = cnt_stable;
    }
//    printf("all_interval.size=%lu stable=%d total=%d\n", all_intervals.size(), cnt_stable, cnt_total);
    auto ret = merge_interval_weighted_mean(all_intervals, time_l, time_r);
    return ret;
}

void print_graph_json(const Graph &graph, int rank, const string &filename_prefix)
{
    // debug
    double sum_comm_time = 0;

    finish_flag = true;
    Json::Value output;
    string filename = filename_prefix + std::to_string(rank) + string(".txt");
    ofstream os;
    os.open(filename.c_str());
    for (const auto& item:graph)
    {
        Json::Value list;
//        cout << get_addr_from_mixture(item.first.first) << " " <<
//             get_status_from_mixture(item.first.first) << " " <<
//             get_addr_from_mixture(item.first.second) << " " <<
//             get_status_from_mixture(item.first.second) << " " <<
        list["info"].append((uint64_t)item.first.first);
        list["info"].append((uint64_t)item.first.second);
        list["ccnt"] = cluster_cnt[item.first];
        int cnt=0;
        vector<uint64_t> temp_sum(6,0);
        const int smooth_len=10;
        for (const auto &data:item.second)
        {
            //for (auto value:data)
            //    one_call.append((uint64_t)value);
            Json::Value one_call;
            for (auto v:data.papi)
                one_call.append((uint64_t)v);
            one_call.append(data.mpi_func);
            one_call.append(data.mpi_count);
            one_call.append(data.target);
            //debug
            one_call.append("T");

            one_call.append(double(data.timestamp - init_time) / CPU_FREQ);
            if (data.mpi_func==J_MPI_INIT)
                one_call.append(-0.00001);
            else
            {
                one_call.append(double(data.elapsed) / CPU_FREQ);
                sum_comm_time += double(data.elapsed) / CPU_FREQ;
            }

            list["value"].append(one_call);
        }
        output["edge"].append(list);
    }
    Json::FastWriter fast_writer;
    os << fast_writer.write(output);
    os.close();
}

void print_function_cnt_to_file(FILE *f)
{
    map<void*, int> cnt_map;
    for (auto kv:graph_comm)
    {
        // CallPath address(hash value) must be different
        if (vertex_type != VertexType::CallPath &&
            !(kv.first.first == 0 ||
              ((long long) kv.first.first) == ((long long) kv.first.second)))
        {

            printf("error: %lld %lld\n", (long long) kv.first.first,
                   (long long) kv.first.second);
            assert(0);
        }
        if (!cnt_map.count(kv.first.second))
            cnt_map[kv.first.second] = 0;
        cnt_map[kv.first.second]+=kv.second.size();
    }
    Json::Value output;
    for (auto kv:cnt_map)
        output[std::to_string((unsigned long long)kv.first)] = kv.second;

    Json::FastWriter fast_writer;
    ostringstream os;
    os << fast_writer.write(output);
    fprintf(f, "%s", os.str().c_str());
}

/**
 * Output graph and original data to file
 * @param rank
 */
void generate_performance_graph(int rank)
{
    summary.total_run_time = (rdtsc() - init_time) / CPU_FREQ;
    int l = 0, r = int(summary.total_run_time);
//    // debug
//    puts("Start calulation for log2/3\n");
    auto local_calc_perf = calc_performance_process(graph_calc, l, r);
    auto local_comm_perf = comm_performance_process(graph_comm, l, r);
    summary.edge_cnt = graph_calc.size();
    summary.vertex_cnt = graph_comm.size();
//    // debug
//    puts("Start prints log2/3\n");
    // i=2 is calc, i=3 is comm
    for (int i=2;i<4;++i)
    {
        string filename = string("log") + to_string(i) + string("_") + std::to_string(rank) + string(".txt");
        auto f = fopen(filename.c_str(), "w");
        fprintf(f, "%d %d %lf %lf\n", l, r, summary.covered_calc_time, summary.covered_comm_time);
        fprintf(f, "%s", summary.generate_summary_json().c_str());
        print_function_cnt_to_file(f);

        for (auto value: (i == 2) ? local_calc_perf : local_comm_perf)
        {
            fprintf(f, "%lf ", value);
            fprintf(f, "\n");
        }
    }
//    // debug
//    puts("Start prints log0/1\n");

    if (mpi_rank==0||mpi_rank==233)
    {
        print_graph_json(graph_calc, rank, "log0_");
        print_graph_json(graph_comm, rank, "log1_");
    }
}

void print_graph(int rank)
{
//    puts("Start print_graph\n");
    generate_performance_graph(rank);
//    puts("End print_graph\n");
}
