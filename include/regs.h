#ifndef __REGS_H__
#define __REGS_H__

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <mem.h>
#include <regstring.h>

enum {
    NRegsBound = 256, /* if changed, update getRegIx */
    NItemsBound = 1024 /*more than enough*/
};

enum { BadCol = SIZE_MAX };

typedef struct {
    /* [data[beg .. end] slot[ix,len]...] */
    Array buf;
    size_t reg[NRegsBound];
    size_t ncols[NRegsBound];
    char* items[NItemsBound];
    regix_t nregs;
} Regs;

typedef struct {
    const char* b;
    size_t sz;
    bool valid;
} QueryResult;

Err initRegs(Mem m[static 1], Regs rc[static 1], size_t sz);
Err regsCopyChunk(Regs regs[static 1], size_t offset[static 1], const char* src, size_t n);
Err readRegs(Regs regs[static 1], const StrView sep);

QueryResult queryRegItem(const Regs r[static 1], regix_t row, size_t col);
Err printQuery(const Regs r[static 1], const char* q);

static inline bool regOutOfRange(const Regs r[static 1], regix_t i) {
    return i >= r->nregs;
}

static inline size_t colsInReg(const Regs r[static 1], regix_t i) {
    if (regOutOfRange(r, i)) { return 0; }
    size_t prev = i ? r->ncols[i-1] : 0;
    return r->ncols[i] - prev;
}

static inline bool colOutOfRange(const Regs r[static 1], regix_t reg, colix_t c) {
    return c >= colsInReg(r, reg);
}

#endif
