#include <iostream>
#include <cstdlib>
#include <ctime>
#include <zconf.h>
#include <cstdlib>
#include <cstdio>
#include <climits>
#include <papi.h>
#include <vector>
#include <map>
#include <set>


using namespace std;
const int size=10000;
int a[size][size];

#define MX 1024
#define NITER 20

#define MEGA 1000000
#define TOT_FLOPS (2*MX*MX*NITER)

/* Get actual CPU time in seconds */
float gettime()
{
    return((float)PAPI_get_virt_usec()*1000000.0);
}

using DataType=std::vector<long long>;
using GraphKey=std::pair<void*,void*>;
using GraphValue=std::vector<DataType >;


static void *last_call_addr = 0;
int cnt_address;
map<void*, int> address_map;
map<GraphKey, GraphValue > graph;
double ad[MX][MX];

const int cnt_events=3;
int events[3] = {PAPI_TOT_INS, PAPI_TOT_CYC, PAPI_LST_INS};

void papi_start_counters()
{
    int ret;
    if ((ret = PAPI_start_counters(events, cnt_events)) != PAPI_OK) {
        fprintf(stderr, "PAPI failed to start counters: %s\n", PAPI_strerror(ret));
        exit(1);
    }
}

void papi_init()
{
    int ret;

    if (PAPI_num_counters() < cnt_events) {
        fprintf(stderr, "No hardware counters here, or PAPI not supported.\n");
        exit(1);
    }
    if ((ret = PAPI_set_domain(PAPI_DOM_USER | PAPI_DOM_KERNEL)) != PAPI_OK)
    {
        fprintf(stderr, "PAPI failed to set_domain: %s\n", PAPI_strerror(ret));
        exit(1);
    }
    papi_start_counters();
}

/** \internal
 * This is stored per thread
 */
typedef struct _HighLevelInfo
{
    int EventSet;					/**< EventSet of the thread */
    short int num_evts;				/**< number of events in the eventset */
    short int running;				/**< STOP, START, or RATE */
    long long initial_real_time;	/**< Start real time */
    long long initial_proc_time;	/**< Start processor time */
    long long last_real_time;		/**< Previous value of real time */
    long long last_proc_time;		/**< Previous value of processor time */
    long long total_ins;			/**< Total instructions */
} HighLevelInfo;

/* high level papi functions*/

/*
 * Which high-level interface are we using?
 * From papi_hl.c
 */
#define HL_START	1
#define HL_READ		6
#define HL_ACCUM	7


/**
 * int _internal_hl_read_cnts( long long *values, int array_len, int flag )
 * @param values
 * @param array_len
 * @param flag
 * @return
 */
int
papi_read_without_reset( long long *values, int array_len, int flag )
{
    int retval;
    HighLevelInfo *state = NULL;

    if ( ( retval = _internal_check_state( &state ) ) != PAPI_OK )
        return ( retval );

    if ( state->running != HL_START || array_len < state->num_evts )
        return ( PAPI_EINVAL );

    if ( flag == HL_ACCUM )
        return ( PAPI_accum( state->EventSet, values ) );
    else if ( flag == HL_READ ) {
        if ( ( retval = PAPI_read( state->EventSet, values ) ) != PAPI_OK )
            return ( retval );
        return ( PAPI_reset( state->EventSet ) );
    }

    /* Invalid flag passed in */
    return ( PAPI_EINVAL );
}


DataType papi_get_data()
{
    long_long values[cnt_events];
    int ret;
    if ( ( ret = PAPI_read( cnt_events, values ) ) != PAPI_OK )
    {
        fprintf(stderr, "PAPI failed to read: %s\n", PAPI_strerror(ret));
        exit(1);
    }

//    if ((ret = PAPI_read_counters(values, cnt_events)) != PAPI_OK) {
//        fprintf(stderr, "PAPI failed to read counters: %s\n", PAPI_strerror(ret));
//        exit(1);
//    }
    return DataType(values, values + cnt_events);
}

void insert_data(void *last_addr, void *current_addr, DataType& papi_data)
{
    if (address_map.count(current_addr))
    {
        address_map[current_addr] = cnt_address++;
    }
    if (address_map.count(last_addr))
    {
        address_map[last_addr] = cnt_address++;
    }
    auto key=make_pair(last_addr,current_addr);
    if (!graph.count(key))
        graph[key]=GraphValue();
    graph[key].push_back(papi_data);
}

__attribute_noinline__ void* get_invoke_point()
{
    return __builtin_return_address(2);
}

__attribute_noinline__ void papi_updata()
{
    auto current_call_addr = get_invoke_point();
    auto papi_data = papi_get_data();
    insert_data(last_call_addr, current_call_addr, papi_data);
    last_call_addr=current_call_addr;
}

void print_graph()
{
    for (const auto& item:graph)
    {
        cout << item.first.first << " " << item.first.second << " " << endl;
        for (const auto &data:item.second)
        {
            for (const auto &value:data)
                printf("%12lld ", value);
            puts("");
        }
    }
}

void kkk()
{
//    void *p = get_invoke_point();
//    cout << p << endl;
    papi_updata();
//    papi_start_counters();
}

void ttt()
{
    kkk();
}

int main ()
{
    papi_init();
    int i;
    i++;
    kkk();
    i++;
    kkk();
    i++;
    kkk();
    i++;
    kkk();
    ttt();
    print_graph();
}

