#ifndef __STORE_H_
#define __STORE_H_

#include <util.h>

const char* getRegfilePath(void);

//Err foreachReg(const char* regList, Err(*preFn)(const char), Err(*chunkFn)(const char*, size_t len));
Err foreachReg(
    const char* regList,
    void(*preFn)(const char),
    void(*chunkFn)(const char*, size_t len),
    void(*postFn)(void)
);
#endif
