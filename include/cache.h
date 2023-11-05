#ifndef __CACHE_H__
#define __CACHE_H__

#include <stddef.h>

#include <mem.h>

typedef struct {
    char* buf;
    size_t bufsz;
    size_t reg[256 /*more than enough*/];
} RegsCache;

#endif
