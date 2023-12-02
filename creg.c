#include <string.h>
#include <stdlib.h>

#include <cache.h>
#include <util.h>
#include <mem.h>
#include <opt-parse.h>
#include <store.h>
#include <regstring.h>
#include <regio.h>


Err testSplit(Mem m[static 1]);
///
void printChunk(const char* chunk, size_t len) {
    file_write(chunk, 1, len, stdout);
}

void printPreReg(const char reg) { printf("\"%c ", reg); }
void printPostLn(void) { puts(""); }
void printPostSpace(void) { printf(" "); }
void regTooLargefn(void) { puts("\t\033[91m""\\...""\033[0m"); }

void skipPre(const char reg) { (void)reg; }

void print_help(const char* prog, const char* err_msg) {
    if (err_msg) {
        fprintfmt(stderr,"%s\n", err_msg);
    }
    fprintfmt(stdout, "usage: %s [-psh] [0-9a-zA-z]\n", prog);
}

Err processQueries(Mem* mem, const char** queries, size_t nqueries, StrView sep) {

    Err e = Ok;
    Regs regs;
    if (!initRegs(mem, &regs, 8000)) {
        e = readRegs(&regs, sep);
        if (e) { return e; }
    }

    for (size_t i = 0; i < nqueries; ++i) {
        const char* q = queries[i];
        Err e = printQuery(&regs, q);
        if (e) { return e; }
        file_write("\n", 1, 1, stdout);
    }
    return e;
}

int main(int argc, const char* argv[]) {

    Mem mem = (Mem) { .tag=DumbMemTag };
    Err e = initMem(&mem);
    if (e) {
        fprintfmt(stderr, "%s\n", "Error memory, aborting.");
        exit(-1);
    }

    e = Ok;

    CliInput* cli = opt_parse(&mem, argc, argv);

    if (cli) {
        switch(cli->tag) {
            case StdinInputTag:
                e = updateRegfile(&mem);
                break;
            case QueriesInputTag:
                e = processQueries(
                    &mem,
                    cli->queries.queries,
                    cli->queries.n,
                    (StrView){.cs=" ", .sz=1}
                );
                if (e != Ok) {
                    fprintfmt(stderr, "error processing queries: ");
                }
                break;
            case QueriesSepInputTag:
                e = processQueries(
                    &mem,
                    cli->queries.queries,
                    cli->queries.n,
                    (StrView){.cs=cli->queriesSep.sep, .sz=strlen(cli->queriesSep.sep)}
                );
                if (e != Ok) {
                    fprintfmt(stderr, "error processing queries: ");
                }
                break;
            case PrintInputTag: 
                e = foreachReg(printPreReg, printChunk, printPostLn);
                break;
            case HelpInputTag:
                print_help(argv[0], "");
                break;
            default: LOG_INVALID_TAG;
        }
        if (e) {
            if (e == -1) {
                fprintfmt(stderr, "aborting\n");
            } else {
                fprintfmt(stderr, "error: %s. Aborting\n", strerror(e));
            }
            exit(e);
        }
    }
}

