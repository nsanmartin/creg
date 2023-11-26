#ifndef __OPT_PARSE_H_
#define __OPT_PARSE_H_

#include <stdlib.h>

#include <mem.h>

typedef enum {
    StdinInputTag,      /* no option no param, read from stdin */
    QueriesInputTag,    /* no option only params (queries)     */
    QueriesSepInputTag, /* -s x option and params (queries)    */
    PrintInputTag,      /* -p opt, no params                   */
    HelpInputTag        /* -h opt, no params                   */
} CliInputTag;

//typedef struct {} StdinInput;
//typedef struct {} HelpInput;
typedef struct { const char** queries; size_t n; } QueriesInput;
typedef struct { const char** queries; size_t n; const char* sep; } QueriesSepInput;
//typedef struct {} PrintInput;

typedef struct {
    CliInputTag tag;
    union {
        QueriesInput     queries;
        QueriesSepInput  queriesSep;
    };
} CliInput;

CliInput* newPrintInput(Mem m[static 1]);
CliInput* opt_parse(Mem m[static 1], int argc, const char* argv[]);
#endif
