#ifndef __OPT_PARSE_H_
#define __OPT_PARSE_H_

#include <stdlib.h>

#include <mem.h>

typedef enum { StdInputTag, RegsInputTag, RegsInputSepTag, PrintInputTag } CliInputTag;

typedef struct {} StdInput;
typedef struct { const char** regs; size_t n; } RegsInput;
typedef struct { const char** regs; size_t n; const char* sep; } RegsInputSep;
typedef struct {} PrintInput;

typedef struct {
    CliInputTag tag;
    union {
        StdInput      std;
        RegsInput     regs;
        RegsInputSep  regsSep;
        PrintInput    print;
    };
} CliInput;

CliInput* newPrintInput(Mem m[static 1]);
CliInput* opt_parse(Mem m[static 1], int argc, const char* argv[]);
#endif
