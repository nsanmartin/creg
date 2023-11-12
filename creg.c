#include <string.h>
#include <stdlib.h>

#include <cache.h>
#include <util.h>
#include <mem.h>
#include <opt-parse.h>
#include <store.h>

;
Err testSplit(Mem m[static 1]);
///
void printChunk(const char* chunk, size_t len) {
    fwrite(chunk, 1, len, stdout);
}

void printPreReg(const char reg) { printf("\"%c ", reg); }
void printPostLn(void) { puts(""); }
void printPostSpace(void) { printf(" "); }
void regTooLargefn(void) { puts("\t\033[91m""\\...""\033[0m"); }

void skipPre(const char reg) { }

void print_help(const char* prog, const char* err_msg) {
    if (err_msg) {
        fprintf(stderr,"%s\n", err_msg);
    }
    fprintf(stdout, "usage: %s [-psh] [0-9a-zA-z]\n", prog);
}

int main(int argc, const char* argv[]) {

    Mem mem = (Mem) { .tag=DumbMemTag };
    Err e = initMem(&mem);
    if (e) {
        fprintf(stderr, "%s\n", "Error memory, aborting.");
        exit(-1);
    }

    e = Ok;

    ///
    //testSplit(&mem); 
    //return -1;
    ///

    CliInput* cli = opt_parse(&mem, argc, argv);
    if (cli) {
        switch(cli->tag) {
            case StdinInputTag:
                e = updateRegfile(&mem);
                break;
            case QueriesInputTag:
                for (int i = 0; i < cli->regs.n; ++i) {
                    if (printRegs(&mem, cli->regs.regs[i]) == Ok) {
                        fwrite("\n", 1, 1, stdout);
                    } else {
                        fprintf(stderr, "Aborting.");
                        break;
                    }
                }
                break;
            case QueriesSepInputTag:
                Regs regs;
                if (!initRegs(&mem, &regs, 8000)) {
                    readRegs(&regs);
                }
                
                size_t from = 0;

                for (size_t i = 0; i < regs.nregs; ++i) {
                    const size_t to = regs.ncols[i];
                    for (size_t j = from; j < to; ++j) {
                        const char* beg = regs.items[j];
                        size_t len = regs.items[j+1] - beg;
                        fwrite(beg, 1, len, stdout);
                        fwrite(" ", 1, 1, stdout);
                    }
                    from = to;
                    fwrite("\n", 1, 1, stdout);
                }
                QueryResult qr = queryRegItem(&regs, 5, 5);
                if (qr.valid) {
                    fwrite(qr.b, 1, qr.sz, stdout);
                }

                break;
            case PrintInputTag: 
                e = foreachReg("", printPreReg, printChunk, printPostLn);
                break;
            case HelpInputTag:
                print_help(argv[0], "");
                break;
            default: LOG_INVALID_TAG;
        }
        if (e) {
            fprintf(stderr, "error: %s. Aborting\n", strerror(e));
            exit(e);
        }
    }
}

