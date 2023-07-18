//
// Created by zheng on 2020/8/6.
//

#include <dlfcn.h>
#include "papi_wrap.h"

//typedef ssize_t (*pread_t)(int fd, void *buf, size_t count, off_t offset);
//extern "C"  ssize_t pread(int fd, void *buf, size_t count, off_t offset) {
//    static pread_t pread_real = NULL;
//    unsigned char *c;
//    int port,ok=1;
//
//    if (!pread_real) pread_real = (pread_t) dlsym(RTLD_NEXT, "pread");
//    printf("pread %d %d %d\n", fd, count, offset);
//    if (ok) return pread_real(fd, buf, count, offset);
//}