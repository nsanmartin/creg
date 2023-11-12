#ifndef __CACHE_H__
#define __CACHE_H__

#include <stddef.h>

#include <regs.h>
#include <mem.h>

typedef struct {
    char* buf;
    size_t bufsz;
    size_t reg[NRegsBound];
} RegsCache;


Err initRegsCache(Mem m[static 1], RegsCache rc[static 1], size_t sz);
Err regsCacheCopyChunk(RegsCache regsCache[static 1], size_t offset[static 1], char* src, size_t n);
SizedBuf regsCacheReg(RegsCache regsCache[static 1], size_t regindex);
#endif
