#ifndef __CACHE_H__
#define __CACHE_H__

#include <stddef.h>

#include <mem.h>

enum { NRegsBound = 256, NItemsBound = 1024 /*more than enough*/ };

typedef struct {
    /* [data[beg .. end] slot[ix,len]...] */
    Array buf;
    size_t reg[NRegsBound];
    size_t regColCount[NRegsBound];
    char* cols[NItemsBound];
    size_t regMax;
} RegsMat;

typedef struct {
    char* buf;
    size_t bufsz;
    size_t reg[NRegsBound];
} RegsCache;


Err initRegsCache(Mem m[static 1], RegsCache rc[static 1], size_t sz);
Err regsCacheCopyChunk(RegsCache regsCache[static 1], size_t offset[static 1], char* src, size_t n);
Err regsMatCopyChunk(RegsMat regsMat[static 1], size_t offset[static 1], const char* src, size_t n);
SizedBuf regsCacheReg(RegsCache regsCache[static 1], size_t regindex);
Err readRegsMat(RegsMat regsMat[static 1]);

Err initRegsMat(Mem m[static 1], RegsMat rc[static 1], size_t sz);
#endif
