#include "pmc_wrapper.h"
// ====== perf ======
#ifdef USE_ASSEMBLY_RDPMC

void pmc_enable()
{
    init_time = rdtsc();

    int i;
    struct perf_event_attr attr;
    int pmcFd;
    int event[1] = {PERF_COUNT_HW_INSTRUCTIONS}; //, PERF_COUNT_HW_CPU_CYCLES, PERF_COUNT_HW_REF_CPU_CYCLES};

    for (i = 0; i < 1; ++i)
    {
        // Configure th event
        memset(&attr, 0, sizeof(struct perf_event_attr));
        attr.type = PERF_TYPE_HARDWARE;
        attr.size = sizeof(struct perf_event_attr);
        attr.config = event[i];
        attr.inherit = 1;

        // Due to the setting of attr.inherit, it will also count all child
        pmcFd = perf_event_open(&attr, 0, -1, -1, 0);
        if (pmcFd < 0)
            fprintf(stderr, "Failed to start [%d]\n", i);

#if 0
        // Resetting counter to zero
    ioctl(pmcFd, PERF_EVENT_IOC_RESET, 0);
#endif
        // Start counters
        ioctl(pmcFd, PERF_EVENT_IOC_ENABLE, 0);
#if 0
        // Stop counter
    //ioctl(pmcFd, PERF_EVENT_IOC_DISABLE, 0);
    // Read result
    long long r1;
    read(pmcFd, &r1, sizeof(long long));
    close(pmcFd);
#endif
    }

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
    long long values[CNT_PAPI_EVENTS];
    values[0] = rdpmc_instructions();
    unsigned long long new_last_totcycle = values[0];
    values[0] = ((unsigned long long)values[0]) - last_totcycle;
    DataType retv;
    retv.set_papi_data(values);
    retv.elapsed = cur_time - last_time; // append wall time to locate this record
    retv.timestamp = cur_time;
    last_time = cur_time;
    last_totcycle = new_last_totcycle;
    return retv;
}

#elif PMC_IOCTL
long pmcFd = -1;

static void pmc_enable()
{
    init_time = rdtsc();

    struct perf_event_attr pe;
    memset(&pe, 0, sizeof(struct perf_event_attr));
    pe.type = PERF_TYPE_HARDWARE;
    pe.size = sizeof(struct perf_event_attr);
    pe.config = PERF_COUNT_HW_INSTRUCTIONS;
    pe.disabled = 1;
    pe.exclude_kernel = 1;
    pe.exclude_hv = 1;

    pmcFd = syscall(__NR_perf_event_open, &pe, 0, -1, -1, 0);
    ioctl(pmcFd, PERF_EVENT_IOC_RESET, 0);
    ioctl(pmcFd, PERF_EVENT_IOC_ENABLE, 0);

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

inline uint64_t rdpmc_instructions()
{
    uint64_t ret = 0;
    read((int)pmcFd, &ret, sizeof(long long));
    ioctl(pmcFd, PERF_EVENT_IOC_RESET, 0);
    return ret;
}

inline DataType pmc_get_data()
{
    long long values[CNT_PAPI_EVENTS];
    values[0] = rdpmc_instructions();
    unsigned long long cur_time = rdtsc();
    DataType retv;
    retv.set_papi_data(values);
    retv.elapsed = cur_time - last_time; // append wall time to locate this record
    retv.timestamp = cur_time;
    last_time = cur_time;
    last_totcycle = values[0];
    return retv;
}

inline void pmc_reset()
{
    ioctl(pmcFd, PERF_EVENT_IOC_RESET, 0);
}
#endif

// ====== papi ======

#ifdef USE_PAPI
void papi_start_counters()
{
    int ret;
    if ((ret = PAPI_start_counters(events, CNT_PAPI_EVENTS)) != PAPI_OK)
    {
        fprintf(stderr, "PAPI failed to start counters: %s\n", PAPI_strerror(ret));
        exit(1);
    }
}

void papi_init()
{
    int ret;
    init_time = rdtsc();
    if (PAPI_num_counters() < CNT_PAPI_EVENTS)
    {
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
    if (mpi_rank == 0)
        fprintf(stderr, "Papi inited: VERTEX_GRAIN=%d\n", vertex_path_int);
    assert(!(vertex_path_int < 0 || vertex_path_int > 2));
    if (vertex_path_int < 0 || vertex_path_int > 2)
    {
        fprintf(stderr, "vertex path environment variable error");
        exit(-1);
    }
    else
        vertex_type = (VertexType)vertex_path_int;
    if (mpi_rank == 0)
    {
        fprintf(stderr, "vertex type: %d\n", (int)vertex_type);
    }

    init_flag = true;
    papi_start_counters();
}

DataType papi_get_data(ULL cur_time)
{
    int ret;
    long long values[CNT_PAPI_EVENTS];
    if ((ret = PAPI_read_counters(values, CNT_PAPI_EVENTS)) != PAPI_OK)
    {
        fprintf(stderr, "PAPI failed to read counters: %s\n", PAPI_strerror(ret));
        exit(1);
    }
    DataType retv;
    retv.set_papi_data(values);
    retv.elapsed = cur_time - last_time; // append wall time to locate this record
    retv.timestamp = cur_time;
    last_time = cur_time;
    return retv;
}
#endif