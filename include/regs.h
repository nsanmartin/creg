#ifndef __REGS_H__
#define __REGS_H__

#include <stddef.h>

#include <mem.h>

enum { NRegsBound = 256, NItemsBound = 1024 /*more than enough*/ };
enum { DataBufSz = 4000 };

typedef struct {
    /* [data[beg .. end] slot[ix,len]...] */
    Array buf;
    size_t reg[NRegsBound];
    size_t ncols[NRegsBound];
    char* items[NItemsBound];
    size_t regMax;
} Regs;

Err initRegs(Mem m[static 1], Regs rc[static 1], size_t sz);
Err regsCopyChunk(Regs regs[static 1], size_t offset[static 1], const char* src, size_t n);
Err readRegs(Regs regs[static 1]);
#endif
