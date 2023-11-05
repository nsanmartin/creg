#include <string.h>
#include <stdlib.h>

#include <util.h>
#include <mem.h>
#include <opt-parse.h>
#include <store.h>

void printChunk(const char* chunk, size_t len) {
    fwrite(chunk, 1, len, stdout);
}

void printPreReg(const char reg) { printf("\"%c ", reg); }
void printPostLn(void) { puts(""); }
void printPostSpace(void) { printf(" "); }
void regTooLargefn(void) { puts("\t\033[91m""\\...""\033[0m"); }

void skipPre(const char reg) { }

int main(int argc, const char* argv[]) {

    Mem mem = (Mem) { .tag=DumbMemTag };
    Err e = initMem(&mem);
    if (e) {
        fprintf(stderr, "%s\n", "Error memory, aborting.");
        exit(-1);
    }

    e = Ok;
    CliInput* cli = opt_parse(&mem, argc, argv);
    if (cli) {
        switch(cli->tag) {
            case StdInputTag:
                e = updateRegfile(&mem);
                break;
            case RegsInputTag:
                for (int i = 0; i < cli->regs.n; ++i) {
                    e = foreachReg(cli->regs.regs[i], skipPre, printChunk, printPostSpace);
                    fwrite("\n", 1, 1, stdout);
                }
                break;
            case RegsInputSepTag:
                puts("qery with sep");
                break;
            case PrintInputTag: 
                e = foreachReg("", printPreReg, printChunk, printPostLn);
                break;
            default: LOG_INVALID_TAG;
        }
        if (e) {
            fprintf(stderr, "error: %s. Aborting\n", strerror(e));
            exit(e);
        }
    }
}

