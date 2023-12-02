#ifndef __REG_STRING_H_
#define __REG_STRING_H_

#include <stdlib.h>

typedef struct { 
    const char* cs;
    size_t sz;
} StrView;

StrView findSubStrViewIx(StrView s, StrView subs);
StrView findNextSubStrOrLastIx(const char* s, StrView sep);
#endif
