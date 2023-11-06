#ifndef __REG_STRING_H_
#define __REG_STRING_H_

#include <stdlib.h>

typedef struct { 
    const char* cs;
    size_t sz;
} StrView;

//typedef strict {
//    const char* cs;
//    size_t* ix;
//    const char* sep;
//    size_t seplen;
//} StrViewSplit;
StrView findSubStrViewIx(StrView s, StrView subs);
#endif
