#ifndef __OPT_PARSE_H_
#define __OPT_PARSE_H_

#include <stdlib.h>

#include <mem.h>

typedef enum {
    StdInputTag, QueriesInputTag, QueriesSepInputTag, PrintInputTag, HelpInputTag
} CliInputTag;

typedef struct {} StdInput;
typedef struct {} HelpInput;
typedef struct { const char** regs; size_t n; } QueriesInput;
typedef struct { const char** regs; size_t n; const char* sep; } QueriesSepInput;
typedef struct {} PrintInput;

typedef struct {
    CliInputTag tag;
    union {
        QueriesInput     regs;
        QueriesSepInput  regsSep;
        //StdInput         std;
        //PrintInput       print;
    };
} CliInput;

CliInput* newPrintInput(Mem m[static 1]);
CliInput* opt_parse(Mem m[static 1], int argc, const char* argv[]);
#endif
