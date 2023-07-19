//
// Created by eric on 19-1-30.
//
//#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>
#include "papi_wrap.h"

int puts(const char *message)
{
    // using PutType=int (*)(const char *message);
    typedef int (*PutType)(const char *message);
    PutType real_puts;
    int ret;
    real_puts = (PutType) dlsym(RTLD_NEXT, "puts");
    papi_update(0);
    ret = real_puts(message);
    papi_update(1);
    return ret;
}

static void con() __attribute__((constructor));

static void des() __attribute__((destructor));

void con()
{
    printf("run constructor %d\n ", 1);
    papi_init();
}

void des()
{
    printf("run destructor %d\n ", 1);
    print_graph();
}

