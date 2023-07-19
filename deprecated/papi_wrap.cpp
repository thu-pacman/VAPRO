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
#include "utility.h"
#include <linux/perf_event.h>

// gettimeofday
//#include<sys/time.h>
#include <unistd.h>
#include <sstream>
#include <syscall.h>
#include <sys/ioctl.h>
#include "online_analyse.h"
#include "SocketClient.h"
// PMU, rdtsc
#include "pmu.h"
#include "pmc_wrapper.h"

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

using namespace std;
const long long Cpu_freq = CPU_FREQ;

// using DataType=std::vector<long long>;
// using GraphKey=std::pair<void*,void*>;
// using GraphValue=std::vector<DataType >;

int events[6] = {PAPI_TOT_INS, PAPI_L1_TCM, PAPI_L2_TCM, PAPI_BR_MSP, PAPI_VEC_DP}; //, PAPI_SR_INS}; //, PAPI_LD_INS, PAPI_L3_TCA};
const int J_MPI_INIT = 0;
const int J_MPI_BARRIER = 23;
const int J_MPI_WAIT = 317;
const int J_MPI_COMM_RANK = 61;
const int J_MPI_COMM_SIZE = 68;
const int J_MPI_WTIME = 359;
const int J_MPI_WAITANY = 319;
const int J_MPI_WAITALL = 318;
const int J_MPI_FILE_WRITE_AT_ALL = 138;
const int J_MPI_FILE_READ_AT_ALL = 118;
const int J_MPI_ALLREDUCE = 16;
const int J_MPI_FILE_READ_AT = 117;
const int J_MPI_FILE_WRITE_AT = 137;
const int J_MPI_Irecv = 4;
const int J_MPI_Iporbe = 208;
const int J_MPI_Testall = 271;
const int J_DYNINST = 999; // Dyninst instrumentation
const set<int> FILTER_OUT_MPI = {J_MPI_INIT, J_MPI_BARRIER, J_MPI_WAIT,
                                 J_MPI_COMM_RANK, J_MPI_COMM_SIZE, J_MPI_WTIME,
                                 J_MPI_WAITANY, J_MPI_WAITALL, J_MPI_Irecv,
                                 J_MPI_Iporbe, J_MPI_Testall};
// const set<int> FILTER_OUT_MPI = {J_MPI_INIT, J_MPI_BARRIER, J_MPI_WAIT, J_MPI_COMM_RANK, J_MPI_COMM_SIZE, J_MPI_WTIME};
const set<int> IO_MPI = {J_MPI_FILE_READ_AT_ALL, J_MPI_FILE_WRITE_AT_ALL, J_MPI_FILE_READ_AT, J_MPI_FILE_WRITE_AT};

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
        output["total_run_time"] = total_run_time;
        output["covered_calc_time"] = covered_calc_time;
        output["covered_comm_time"] = covered_comm_time;
        output["calc_covered_ratio"] = covered_calc_time / total_run_time;
        output["comm_covered_ratio"] = covered_comm_time / total_run_time;
        output["total_covered_ratio"] = (covered_calc_time + covered_comm_time) / total_run_time;
        output["edge_cnt"] = edge_cnt;
        output["vertex_cnt"] = vertex_cnt;

        Json::FastWriter fast_writer;
        os << fast_writer.write(output);
        return os.str();
    }
} summary;

static void *last_call_addr = nullptr;
int cnt_address;
map<void *, int> address_map;
map<void *, int> address_to_func;
map<void *, DataType> last_data_map;
Graph graph_calc, graph_comm, graph_io;
// counter of clustering result
map<GraphKey, int> cluster_cnt;
bool init_flag = false;
// finish flag is necessary because of the occurrence of hooked function in print_graph
bool finish_flag = false;
// ignore recursively invoked MPI function. For instance, PMPI_File_set_view
// invokes MPI_Type_size_x.
bool in_mpi_flag = false;
int recursive_depth = 0;

// switch for online analysis
constexpr bool online_analyze_flag = false;
// switch for analysis server
constexpr bool online_server_flag = false;
// switch for sampling
//#define ENABLE_SAMPLING
constexpr bool sampling_flag = false;
double sampling_rate = 0.01;

int mpi_rank = -1, mpi_size = -1;
VertexType vertex_type = VertexType::CallSite;

// online anaylsis
Graph graph_last_calc, graph_last_comm, graph_last_io;
int env_vapro_online_period = 5; // unit: second
// server analysis
int env_vapro_server_period = 15; // unit: second
SocketClient *socketClient;
char *server_addr;
int server_port;

unsigned long long init_time;
unsigned long long last_time;
// unsigned long long last_totcycle = 0;
vector<ULL> last_pmcs(CNT_PAPI_EVENTS, 0);

void pmc_enable()
{
    init_time = rdtsc();

    // wrapper for pmc API
    pmc_enable_real();

    // get VERTEX_GRAIN
    char *vertex_path_str = getenv("VERTEX_GRAIN");
    int vertex_path_int = (vertex_path_str == 0) ? VertexType::CallSite : atoi(vertex_path_str);
    assert(!(vertex_path_int < 0 || vertex_path_int > 2));
    if (vertex_path_int < 0 || vertex_path_int > 2)
    {
        fprintf(stderr, "vertex path environment variable error");
        exit(-1);
    }
    else
        vertex_type = (VertexType)vertex_path_int;
    init_flag = true;
}

/**
 * This function has side effect. It changes last_time and last_totcycle.
 * @param cur_time
 * @return
 */
inline DataType pmc_get_data(ULL cur_time)
{
    auto new_pmcs = pmc_read_real();
    ULL diff_pmcs[CNT_PAPI_EVENTS];
    for (int i = 0; i < CNT_PAPI_EVENTS; ++i)
        diff_pmcs[i] = new_pmcs[i] - last_pmcs[i];
    DataType retv;
    retv.set_papi_data(diff_pmcs);
    retv.elapsed = cur_time - last_time; // append wall time to locate this record
    retv.timestamp = cur_time;
    last_time = cur_time;
    last_pmcs = std::move(new_pmcs);
    return retv;
}

void vapro_init()
{
    PMPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    PMPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
    // DEBUG
    //    char *env_vapro_online_period = getenv("VAPRO_ONLINE_PERIOD");

    if (mpi_rank == 0)
    {
        fprintf(stderr, "Shared library loaded..\n");
        fprintf(stderr, "INFO: vertex_path_int = %d\n", vertex_type);
    }
    if (online_analyze_flag)
    {
        if (mpi_rank == 0)
            fprintf(stderr, "INFO: online mode ON. period=%d\n", env_vapro_online_period);
        online_anaylze_init();
    }
    else if (online_server_flag)
    {
        server_addr = getenv("VAPRO_SERVER_IP");
        if (!server_addr)
        {
            fprintf(stderr, "VAPRO_SERVER_IP is not defined.\n");
            exit(-1);
        }
        const char *vapro_server_port = getenv("VAPRO_SERVER_PORT");
        if (!vapro_server_port)
        {
            fprintf(stderr, "VAPRO_SERVER_PORT is not defined.\n");
            exit(-1);
        }
        server_port = atoi(vapro_server_port);
        if (mpi_rank == 0)
        {
            fprintf(stderr, "INFO: online server ON. period=%d. Server=%s:%d\n",
                    env_vapro_server_period, server_addr, server_port);
        }
        socketClient = new SocketClient(server_addr, server_port);
    }
    else
    {
        if (mpi_rank == 0)
            fprintf(stderr, "INFO: online mode OFF.\n");
    }
}

void insert_data(map<GraphKey, GraphValue> &graph, void *last_addr, void *current_addr, DataType &papi_data)
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
        graph[key] = GraphValue();
    graph[key].push_back(papi_data);

    //    // differentiate to calculate self loop data
    //    for (int i = 0; i < CNT_PAPI_EVENTS+1; ++i)
    //    {
    //        accumulate_data[i] += papi_data[i];
    //    }
    //
    //    // insert self loop value
    //    if (last_addr != current_addr && last_data_map.count(current_addr))
    //    {
    //        DataType self_loop_data{accumulate_data};
    //        for (int i = 0; i < CNT_PAPI_EVENTS+1; ++i)
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

void output_backtrace()
{
    puts("=====");
    backtrace_symbols_fd(backtrace_buffer, trace_size, 1);
}

__attribute_noinline__ void *get_invoke_point()
{
    // This compiler function(MACRO?) have some strange problems
    // return __builtin_return_address(2);
    //     constexpr int size = 5;
    //  todo: revert
    static bool a = vertex_type == VertexType::CallSite;
    //    static int cnt=0; //Debug
    if (likely(a))
    //    if (false)
    {
        // Upd1: UNROLL the loop. Loop may introduce 50% overhead. Unrolling can reduce it to 10%.

        // // Irregualr stack
        // for (int i = 1; i < trace_size; ++i)
        //     if (((unsigned long long)backtrace_buffer[i]) < 0x1d627d61)
        //         return backtrace_buffer[i];
        return backtrace_buffer[1]; // for HPL and other apps
        //    return backtrace_buffer[2]; // for AMG
    }
    else if (vertex_type == VertexType::CallPath)
    {
        return (void *)hash_sequence(trace_size, backtrace_buffer);
    }

    //    printf("==ERROR===\n");
    //    for (int j = 0; j < trace_size; ++j)
    //    {
    //        printf("d%d=%lx ch=%d\n", j, (unsigned long) buffer[j],
    //               ((unsigned long long) buffer[j] < 0x00645cbdeb8eLL));
    //    }
    //    printf("=====\n");

    output_backtrace();
    fprintf(stderr, "Error: Backtrace do not find EXE SEG call stack\n");
    assert(0);
    exit(1);
}

/**
 * @param suffix should be 0 or 1. 0 is before MPI invocation and 1 is after.
 */
__attribute_noinline__ void papi_update(int suffix, int mpi_func, int count, int target, void *mpi_comm)
{
    if (!init_flag)
    {
        // #ifdef USE_PAPI
        //         papi_init();
        // #else
        pmc_enable();
        // #endif
        // fprintf(stderr, "INFO: start online init, rank=%d\n", mpi_rank);
        int inited = 0;
        MPI_Initialized(&inited);
        //        if (inited && suffix==0)
        // The wrapper of MPI_Init has only suffix 1
        if (inited)
            vapro_init();
        else
            return;
        if (mpi_rank == 0)
            fprintf(stderr, "INFO: Vapro init, rank=%d\n", mpi_rank);
    }
    // if finished
    if (finish_flag)
        return;

    // Get PMU data ASAP to exclude Vapro noise
    unsigned long long current_tsc = rdtsc();
    auto papi_data = pmc_get_data(current_tsc);
#ifdef USE_RUSAGE
    papi_data.set_rusage_data(get_rusage_data().data());
#endif
    papi_data.mpi_func = mpi_func;
    papi_data.mpi_count = count;
    papi_data.target = target;
    papi_data.mpi_comm = mpi_comm;

    // skip in recursive mpi invocations
    // There might be recursive in MPI libraries.
    if (in_mpi_flag && suffix == 0)
    {
        recursive_depth++;
        return;
    }
    if (recursive_depth > 0 && suffix == 1)
    {
        recursive_depth--;
        return;
    }
    in_mpi_flag = (suffix == 0);

    // online analysis
    static unsigned long long last_online_tsc = current_tsc;
    static int num_iters = 0;
    if (online_analyze_flag && init_flag)
    {
        if (current_tsc > last_online_tsc +
                              (unsigned long)(env_vapro_online_period * CPU_FREQ))
        {
            if (num_iters)
            {
                online_analyze(current_tsc, last_online_tsc);
            }
            online_irecv_all();
            online_isend_all(graph_calc, graph_comm, graph_io, last_online_tsc,
                             current_tsc);
            graph_last_calc = std::move(graph_calc);
            graph_last_comm = std::move(graph_comm);
            graph_last_io = std::move(graph_io);
            graph_calc.clear();
            graph_comm.clear();
            graph_io.clear();
            last_online_tsc = current_tsc;
            ++num_iters;
        }
    }

    // server analysis
    if (online_server_flag && init_flag)
    {
        if (current_tsc > last_online_tsc +
                              (unsigned long)(env_vapro_server_period * CPU_FREQ))
        {
            socketClient->serialize(mpi_rank, last_online_tsc - init_time,
                                    current_tsc - init_time, graph_calc,
                                    graph_comm, graph_io);
            graph_calc.clear();
            graph_comm.clear();
            graph_io.clear();
            socketClient->send();
            last_online_tsc = current_tsc;
        }
    }

    long long real_addr;
    if (suffix == 1 && last_call_addr)
    {
        real_addr = (long long)last_call_addr;
    }
    else
    {
        if (mpi_func == J_DYNINST)
            real_addr = -target;
        else
            real_addr = (vertex_type == VertexType::Function) ? mpi_func : (long long)get_invoke_point();
    }
    void *current_call_addr = (void *)(real_addr);
#ifdef ENABLE_SAMPLING
    if (sampling_flag && (double(rand_longlong()) > sampling_rate * ULLONG_MAX))
#endif
        if (!(suffix == 1 && FILTER_OUT_MPI.count(mpi_func)))
        {
            if (suffix == 0)
            {
                if (!(last_call_addr == current_call_addr &&
                      FILTER_OUT_MPI.count(mpi_func)))
                {
                    insert_data(graph_calc, last_call_addr, current_call_addr,
                                papi_data);
                }
            }
            else
            {
                if ((IO_MPI.count(mpi_func)))
                {
                    insert_data(graph_io,
                                last_call_addr, current_call_addr, papi_data);
                }
                else
                {
                    // TODO:  collective online
                    insert_data(graph_comm,
                                last_call_addr, current_call_addr, papi_data);
                }
            }
        }
    last_call_addr = current_call_addr;

    // reset base counter
    last_time = rdtsc();
    // #ifdef USE_PAPI
    //     papi_get_data(last_time); // reset PAPI counter
    // #else
    //     last_totcycle = rdpmc_instructions();
    // #ifndef USE_ASSEMBLY_RDPMC
    //     pmc_reset();
    // #endif
    // #endif
    last_pmcs = pmc_read_real();
    last_time = rdtsc();
}

void *get_addr_from_mixture(void *p)
{
    return (void *)(((long)p) >> 1);
}

int get_status_from_mixture(void *p)
{
    return (int)(((long)p) & 1);
}

struct Interval
{
    double l, r;
    double perf;
};

typedef vector<Interval> PathPerformance;
/**
 * Calculate relative performance from snippets records
 * @param path_data
 * @param smooth: enable smoothing or not
 * @return
 */
PathPerformance calc_path(GraphValue &path_data, bool smooth)
{
    unsigned long long min_elapsed = ULONG_LONG_MAX;
    PathPerformance ret;
    Interval interval;

    const int window = 16; // window size of smoothing
    vector<ULL> smoothed;
    ULL sum = 0;

    // if size is enough, smooth the results
    if (path_data.size() > window && smooth)
    {
        for (int i = 0; i < window; ++i)
        {
            sum += path_data[i].elapsed;
        }
        smoothed.push_back(sum);
        for (auto i = window; i < path_data.size(); ++i)
        {
            sum += path_data[i].elapsed - path_data[i - window].elapsed;
            smoothed.push_back(sum);
        }
        for (auto &v : smoothed)
            min_elapsed = min(v, min_elapsed);
        for (int i = 0; i < path_data.size() - window; ++i)
        {
            const auto &value = path_data[i];
            interval.perf = min_elapsed / double(smoothed[i]);
            interval.l = double(value.timestamp - value.elapsed - init_time) /
                         CPU_FREQ;
            interval.r = double(value.timestamp - init_time) / CPU_FREQ;
            ret.push_back(interval);
        }
    }
    else
    {
        for (const auto &value : path_data)
            min_elapsed = (min_elapsed <= value.elapsed) ? min_elapsed : value.elapsed;
        for (const auto &value : path_data)
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
    }

    return ret;
}

bool online_is_same_calc_cluster(double mean_cycles, double remote_mean_cycles)
{
    return (1 - 0.02) * mean_cycles < remote_mean_cycles &&
           remote_mean_cycles < (1 + 0.02) * mean_cycles;
}

bool online_is_same_comm_cluster(double mpi_count, double remote_mpi_count)
{
    return fabs(remote_mpi_count - mpi_count) < eps;
}

PathPerformance online_calc_path(bool is_calc, GraphValue &path_data,
                                 vector<pair<double, double>> &interprocess_min_time,
                                 bool enable_output = false)
{
    unsigned long long min_elapsed = LONG_LONG_MAX;
    PathPerformance ret;
    Interval interval;
    double mean_cycles = 0;
    int cnt_interprocess_make_effective = 0; // debug
    for (const auto &value : path_data)
    {
        min_elapsed = (min_elapsed <= value.elapsed) ? min_elapsed
                                                     : value.elapsed;
        mean_cycles += value.papi[I_PAPI_TOT_INS];
    }
    assert(path_data.size());
    mean_cycles /= path_data.size();
    ULL local_min_elapsed = min_elapsed;
    //    // debug
    //    if (mpi_rank == 1)
    //    {
    //        fprintf(stderr, "local path_mean=%.3lf min_time=%.3lf\n", mean_cycles,
    //                min_elapsed / CPU_FREQ);
    //        for (auto &tt:interprocess_min_time)
    //        {
    //            fprintf(stderr, "gloabl mean=%.3lf min_time=%.3lf\n", tt.first, tt.second);
    //        }
    //    }

    for (const auto &record : interprocess_min_time)
    {
        if ((is_calc &&
             online_is_same_calc_cluster(mean_cycles, record.first)) ||
            (!is_calc &&
             online_is_same_comm_cluster(path_data[0].mpi_count, record.first)))
        {
            if (min_elapsed > (unsigned long long)(record.second * CPU_FREQ))
            {
                ++cnt_interprocess_make_effective;
                min_elapsed = (unsigned long long)(record.second * CPU_FREQ);
            }
        }
    }

    // debug
    if (mpi_rank == 0 && enable_output)
        fprintf(stderr,
                "DEBUG: among %lu cnt_interprocess_make_effective=%d lt=%llu gt=%llu\n",
                interprocess_min_time.size(), cnt_interprocess_make_effective,
                local_min_elapsed, min_elapsed);

    for (const auto &value : path_data)
    {
        interval.perf = min_elapsed / double(value.elapsed);
        interval.l =
            double(value.timestamp - value.elapsed - init_time) / CPU_FREQ;
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
    return path_data.size() >= 5;
}

bool path_stable(const GraphValue &path_data)
{
    const double variance = 0.04;
    long long inst_min = LONG_LONG_MAX, inst_max = 0;

    for_each(path_data.begin(), path_data.end(),
             [&inst_min, &inst_max](const DataType &data)
             {
                 inst_min = min(inst_min, data.papi[I_PAPI_TOT_INS]);
                 inst_max = max(inst_max, data.papi[I_PAPI_TOT_INS]);
             });
    return (inst_max - inst_min) / (double(inst_max + inst_min) / 2) < variance;
}

vector<double> merge_interval_arithmetic_mean(vector<Interval> &intervals, int l, int r)
{
    sort(intervals.begin(), intervals.end(), [](Interval a, Interval b)
         { return a.l < b.l; });
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
    sort(ret.begin(), ret.end(), [&intervals](int a, int b)
         { return intervals[a].r < intervals[b].r; });

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
    sort(intervals.begin(), intervals.end(), [](Interval a, Interval b)
         { return a.l < b.l; });
    //    // debug
    //    if (mpi_rank == 1)
    //    {
    //        printf("l=%d r=%d\n", l, r);
    //        printf("intervals len=%lu\n", intervals.size());
    //        for_each(intervals.begin(), intervals.end(), [](Interval interval) {
    //            printf("%lf %lf %lf\n", interval.l, interval.r, interval.perf);
    //        });
    //    }
    double sum = 0, partial_sum = 0, partial_cnt = 0;
    int cnt = 0;
    auto rit = intervals.cbegin();
    vector<double> ret;
    vector<int> rank_r;
    int lit = 0;
    for (int i = 0; i < intervals.size(); ++i)
        rank_r.push_back(i);
    sort(ret.begin(), ret.end(), [&intervals](int a, int b)
         { return intervals[a].r < intervals[b].r; });

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
        }
        else
        { // -1 means this interval is not covered
            ret.push_back(-0.005);
        }
    }
    return ret;
}

/**
 * Snippet graph to time-based process relative performance
 * @param graph
 * @param time_l
 * @param time_r
 * @return
 */
vector<double> calc_performance_process(Graph &graph, int time_l, int time_r)
{
    vector<Interval> all_intervals;
    vector<pair<double, double>> *interprocess_min_time = nullptr;
    for (auto &kv : graph)
    {
        vector<vector<DataType>> classified = calc_classify(kv.second, 0.02);
        int cnt_stable = 0, cnt_total = 0;
        //        vector<vector<DataType>> classified = calc_classify_fake(kv.second, 0.02);
        if (online_analyze_flag && interprocess_info.count(kv.first))
            interprocess_min_time = &interprocess_info[kv.first];
        else
            interprocess_min_time = nullptr;
        for (auto &v : classified)
        {
            if (path_data_adequate(v) && path_stable(v))
            {
                cnt_stable++;
                PathPerformance new_intervals;
                if (interprocess_min_time)
                    new_intervals = online_calc_path(true, v, *interprocess_min_time);
                else
                    new_intervals = calc_path(v, false);
                // statistical data
                for (auto &vv : new_intervals)
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

Json::Value commbreakdownOutput;
void dump_addr_interval_map(
    const std::map<pair<void *, void *>, vector<PathPerformance>> &data)
{
    Json::Value output;

    for (const auto &kv : data)
    {
        Json::Value key;
        Json::Value value;

        key.append((Json::UInt64)kv.first.first);
        key.append((Json::UInt64)kv.first.second);
        for (const auto &v : kv.second)
        {
            Json::Value pathPerformance;
            for (auto vv : v)
            {
                Json::Value interval;
                interval.append(vv.l);
                interval.append(vv.r);
                interval.append(vv.perf);
                pathPerformance.append(interval);
            }
            value.append(pathPerformance);
        }
        output.append(key);
        output.append(value);
    }
    commbreakdownOutput.append(output);
}

/**
 * Output commbreakdownOutput to file
 * @param filename_prefix
 */
void dump_addr_interval_map_to_file(string filename_prefix)
{
    string filename = filename_prefix + std::to_string((long long)mpi_rank) +
                      string(".txt");
    ofstream os;
    os.open(filename.c_str());

    Json::FastWriter fast_writer;
    os << fast_writer.write(commbreakdownOutput);
    commbreakdownOutput.clear();
    os.close();
}

vector<double> comm_performance_process(Graph &graph, int time_l, int time_r,
                                        bool enable_output = false)
{
    vector<Interval> all_intervals;
    vector<pair<double, double>> *interprocess_min_time = nullptr;
    std::map<pair<void *, void *>, vector<PathPerformance>> debug_data;

    for (auto &kv : graph)
    {
        vector<vector<DataType>> classified = comm_classify(kv.second);
        //        vector<vector<DataType>> classified = comm_classify_fake(kv.second);
        unsigned long newlen = 0;
        int cnt_stable = 0, cnt_total = 0;
        debug_data[kv.first] = vector<PathPerformance>();

        if (online_analyze_flag && interprocess_info.count(kv.first))
            interprocess_min_time = &interprocess_info[kv.first];
        else
            interprocess_min_time = nullptr;

        // debug
        if (mpi_rank == 0 && enable_output)
        {
            printf("IO %lu classes, total interprocess size=%lu, this snippet %lu, key=%p,%p\n",
                   classified.size(),
                   interprocess_info.size(),
                   interprocess_min_time ? interprocess_min_time->size() : -1,
                   kv.first.first, kv.first.second);
        }

        for (auto &v : classified)
        {
            if (mpi_rank == 0 && enable_output)
            {
                printf("IO class has %lu eles\n", v.size());
            }
            if (path_data_adequate(v))
            {
                // debug: print timing to be calced in python
                //                if (mpi_rank==0) print_timing_for_python(v);
                cnt_stable++;
                PathPerformance new_intervals;
                if (interprocess_min_time)
                    new_intervals = online_calc_path(false, v, *interprocess_min_time,
                                                     enable_output);
                else
                    new_intervals = calc_path(v, true);
                // statistical data
                for (auto &vv : new_intervals)
                {
                    summary.covered_comm_time += vv.r - vv.l;
                    newlen++;
                }
                all_intervals.insert(all_intervals.end(), new_intervals.begin(),
                                     new_intervals.end());
                // TODO: DEBUG dump {kv:interval}
                debug_data[kv.first].push_back(new_intervals);
                //                printf("BD: %d")
            }
            cnt_total++;
        }
        assert(kv.second.size() >= newlen);
        cluster_cnt[kv.first] = cnt_stable;
    }
    auto ret = merge_interval_weighted_mean(all_intervals, time_l, time_r);

    return ret;
}

void print_graph_json(const Graph &graph, int rank, const string &filename_prefix)
{
    // debug
    double sum_comm_time = 0;

    finish_flag = true;
    Json::Value output;
    string filename = filename_prefix + std::to_string((long long)rank) + string(".txt");
    ofstream os;
    os.open(filename.c_str());
    for (const auto &item : graph)
    {
        Json::Value list;
        //        cout << get_addr_from_mixture(item.first.first) << " " <<
        //             get_status_from_mixture(item.first.first) << " " <<
        //             get_addr_from_mixture(item.first.second) << " " <<
        //             get_status_from_mixture(item.first.second) << " " <<
        list["info"].append((Json::UInt64)item.first.first);
        list["info"].append((Json::UInt64)item.first.second);
        // number of clusters with corresponding snippet
        list["ccnt"] = cluster_cnt[item.first];
        vector<uint64_t> temp_sum(6, 0);
        for (const auto &data : item.second)
        {
            // for (auto value:data)
            //     one_call.append((uint64_t)value);
            Json::Value one_call;
            // only place TOT_INST at the beginning
            one_call.append((Json::UInt64)data.papi[0]);
            one_call.append(data.mpi_func);
            one_call.append(data.mpi_count);
            one_call.append(data.target);
            // debug
            one_call.append("T");

            one_call.append(double(data.timestamp - init_time) / CPU_FREQ);
            if (data.mpi_func == J_MPI_INIT)
                one_call.append(-0.00001);
            else
            {
                one_call.append(double(data.elapsed) / CPU_FREQ);
                sum_comm_time += double(data.elapsed) / CPU_FREQ;
            }
            // append other PMU data to the end
            for (int i = 1; i < CNT_TOTAL_EVENTS; ++i)
                one_call.append((Json::UInt64)data.papi[i]);

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
    map<void *, int> cnt_map;
    for (auto kv : graph_comm)
    {
        // CallPath address(hash value) must be different
        if (vertex_type != VertexType::CallPath &&
            !(kv.first.first == 0 ||
              ((long long)kv.first.first) == ((long long)kv.first.second)))
        {

            printf("error: %lld %lld\n", (long long)kv.first.first,
                   (long long)kv.first.second);
            assert(0);
        }
        if (!cnt_map.count(kv.first.second))
            cnt_map[kv.first.second] = 0;
        cnt_map[kv.first.second] += kv.second.size();
    }
    Json::Value output;
    for (auto kv : cnt_map)
        output[std::to_string((unsigned long long)kv.first)] = kv.second;

    Json::FastWriter fast_writer;
    ostringstream os;
    os << fast_writer.write(output);
    fprintf(f, "%s", os.str().c_str());
}

void print_process_memory_layout(int rank, const string &prefix)
{
    //    cat /proc/self/maps
    auto pid = getpid();
    string command = "cat /proc/" + std::to_string(pid) + "/maps";
    command += string(" > ") + prefix + std::to_string((long long)rank) +
               string(".txt");
    system(command.c_str());
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
    auto local_io_perf = comm_performance_process(graph_io, l, r);
    summary.edge_cnt = graph_calc.size();
    summary.vertex_cnt = graph_comm.size() + graph_io.size();
    //    // debug
    //    puts("Start prints log2/3\n");
    // i=2 is calc, i=3 is comm, i=4 is IO
    for (int i = 2; i < 4; ++i)
    {
        string filename =
            string("log") + to_string((long long)i) + string("_") +
            std::to_string((long long)rank) + string(".txt");
        auto f = fopen(filename.c_str(), "w");
        fprintf(f, "%d %d %lf %lf\n", l, r, summary.covered_calc_time,
                summary.covered_comm_time);
        fprintf(f, "%s", summary.generate_summary_json().c_str());
        print_function_cnt_to_file(f);

        for (auto value : (i == 2) ? local_calc_perf : ((i == 3) ? local_comm_perf : local_io_perf))
        {
            fprintf(f, "%lf ", value);
            fprintf(f, "\n");
        }
    }
    //    // debug
    //    puts("Start prints log0/1\n");

    // enable DETAILED segmented output to detect the source of variance
    // DEBUG: 为了减小CG的输出. 用于debug
    //    if (0) // disable detailed output
    //    if (mpi_rank < 64)
    //       if (mpi_rank % 512 == 0)
    {
        //        print_process_memory_layout(rank, "log6_");
        print_graph_json(graph_calc, rank, "log0_");
        print_graph_json(graph_comm, rank, "log1_");
        //        print_graph_json(graph_io, rank, "log5_");
    }
}

void print_graph(int rank)
{
    //    puts("Start print_graph\n");
    //    unsigned long long finalize_time = rdtsc();
    //    // DEBUG: only when slower than 17s output the file
    //    // 17 is too strict because of small variance
    //    if (finalize_time-init_time>19*CPU_FREQ)
    //    {
    //        if (mpi_rank==0)
    //        {
    //            fprintf(stderr, "Vapro: measured elapsed time: %.4f\n",
    //                    double(finalize_time - init_time) / CPU_FREQ);
    //        }
    generate_performance_graph(rank);
    //    }
    //    puts("End print_graph\n");
}

/**
 *
 * @param local_perf
 * @return return NaN if there is no data.
 */
double calc_perf_mean(const vector<double> &local_perf)
{
    int cnt_perf = 0;
    double mean_perf = 0;
    for (auto v : local_perf)
        if (v >= 0)
        {
            mean_perf += v;
            ++cnt_perf;
        }
    return mean_perf / cnt_perf;
}

/**
 * Output graph and original data to file
 * @param rank
 */
void online_generate_latest_period(ULL current_tsc, ULL last_online_tsc)
{
    summary.total_run_time = (current_tsc - last_online_tsc) / CPU_FREQ;
    double l = (last_online_tsc - init_time) / CPU_FREQ,
           r = (current_tsc - init_time) / CPU_FREQ;
    //    // debug
    //    puts("Start calulation for log2/3\n");
    //    printf("Online analyze step mpi_rank=%d\n", mpi_rank);
    auto local_calc_perf = calc_performance_process(graph_calc, l, r);
    auto local_comm_perf = comm_performance_process(graph_comm, l, r);
    auto local_io_perf = comm_performance_process(graph_io, l, r, true);
    summary.edge_cnt = graph_last_calc.size();
    summary.vertex_cnt = graph_last_comm.size() + graph_last_io.size();
    double mean_calc_perf = 0, mean_comm_perf = 0, mean_io_perf = 0;
    mean_calc_perf = calc_perf_mean(local_calc_perf);
    mean_comm_perf = calc_perf_mean(local_comm_perf);
    mean_io_perf = calc_perf_mean(local_io_perf);

    // detailed output under bad performance
    //    if (mean_calc_perf < 0.5 || mean_comm_perf < 0.01 ||
    //        (mean_io_perf < 0.5 && mean_io_perf > 0))
    // DEBUG
    if (false)
    //    if (mpi_rank == 15 || mpi_rank == 0)
    {
        puts(">>>>>>>>>>>>>>>>");
        printf("mpi_rank=%d, time_l=%.2lf, time_r=%.2lf || AVE_calc_perf=%.4lf comm_perf=%.4lf io=%.4lf || #edge=%d, #vertex=%d\n",
               mpi_rank, l, r, mean_calc_perf, mean_comm_perf, mean_io_perf,
               summary.edge_cnt, -1);
        for (auto v : local_calc_perf)
            printf("%.3lf ", v);
        puts("");
        for (auto v : local_comm_perf)
            printf("%.3lf ", v);
        puts("");
        for (auto v : local_io_perf)
            printf("%.3lf ", v);
        puts("\n<<<<<<<<<<<<<<<<");
        // suppress output
        // BUG: stop following output
        //        if (mpi_rank > 22)
        //        {
        //            print_graph_json(graph_calc, mpi_rank,
        //                             string("log0_") + to_string(int(l)) + "-" +
        //                             to_string(int(r)) + "_");
        //            print_graph_json(graph_comm, mpi_rank,
        //                             string("log1_") + to_string(int(l)) + "-" +
        //                             to_string(int(r)) + "_");
        //            print_graph_json(graph_io, mpi_rank,
        //                             string("log4_") + to_string(int(l)) + "-" +
        //                             to_string(int(r)) + "_");
        //            dump_addr_interval_map_to_file(
        //                    string("log9_") + to_string(int(l)) + "-" +
        //                    to_string(int(r)) + "_");
        //        }
    }
}