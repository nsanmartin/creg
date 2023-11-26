#ifndef __MEM_H_
#define __MEM_H_

#include <stddef.h>

#include <util.h>

enum { FixedSzGroupMemSz = 100 };
typedef enum { DumbMemTag, FixedSzGroupMemTag } MemTag;
//typedef struct {} DumbMem;
typedef struct {
    void* ptr[FixedSzGroupMemSz];
    size_t n;
} FixedSzGroupMem;

typedef struct {
    MemTag tag;
    union {
        //DumbMem         dumb;
        FixedSzGroupMem fixedSzGroup;
    };
} Mem;

Err initMem(Mem m[static 1]);
void* memAlloc(Mem m[static 1], size_t n);
Err memCleanup(Mem m[static 1]);
#endif

