#ifndef __STORE_H_
#define __STORE_H_

#include <stdbool.h>

#include <util.h>
#include <mem.h>

const char* getRegfilePath(void);

//Err foreachReg(const char* regList, Err(*preFn)(const char), Err(*chunkFn)(const char*, size_t len));
Err foreachReg(
    const char* regList,
    void(*preFn)(const char),
    void(*chunkFn)(const char*, size_t len),
    void(*postFn)(void)
);

//char* readRegfile(Mem m[static 1]);
Err updateRegfile(Mem m[static 1]);
Err printRegs(Mem m[static 1], const char* regs);
Err printRegsSplit(Mem m[static 1], const char* regs, const char* s);
static inline bool isRegIxValid(regix_t ix) { return ix >= 0; }
#endif
