#ifndef __STORE_H_
#define __STORE_H_

#include <util.h>

const char* getRegfilePath(void);

Err foreachReg(Err(*preFn)(const char), Err(*chunkFn)(const char*));
#endif
