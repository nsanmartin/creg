#ifndef __UTIL_H_
#define __UTIL_H_

#include <stdio.h>

#define LOG_INVALID_TAG                                                \
        fprintf(                                                       \
            stderr,                                                    \
            "\033[91m"                                                 \
            "creg fatal error:\n================"                      \
            "\033[0m"                                                  \
            "\n\tInvalid Tag.\n"                                       \
                "file: %s"                                             \
                ":%d\n"                                                \
                "func: %s\n",                                          \
                __FILE__,                                              \
                __LINE__,                                              \
                __func__)

typedef int Err;
typedef int regix_t;
typedef struct { char* data; size_t sz; } Array;
typedef struct { char* buf; size_t sz; Err e; } SizedBuf;

enum { Ok = 0 } ;
#endif
