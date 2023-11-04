#include <stdlib.h>

#include <util.h>
#include <mem.h>
#include <opt-parse.h>
#include <store.h>

Err printChunk(const char* ln) { int r = printf("%s", ln); return r > 0 ? 0 : r; }
Err printPreReg(const char reg) { int r = printf("\"%c ", reg); return r > 0 ? 0 : r; }
void regTooLargefn(void) { puts("\t\033[91m""\\...""\033[0m"); }
Err printRegs(void) { foreachReg(printPreReg, printChunk); }

int main(int argc, const char* argv[]) {

    Mem mem = (Mem) { .tag=DumbMemTag };
    Err e = initMem(&mem);
    if (e) {
        fprintf(stderr, "%s\n", "Error memory, aborting.");
        exit(-1);
    }

    CliInput* cli = opt_parse(&mem, argc, argv);
    if (cli) {
        switch(cli->tag) {
            case StdInputTag:
                puts("read stdin");
                return 0;
            case RegsInputTag:
                puts("query");
                return 0;
            case RegsInputSepTag:
                puts("qery with sep");
                return 0;
            case PrintInputTag: 
                Err e = printRegs();
                if (e) {
                    fprintf(stderr, "%s\n", "Error processing regs, aborting.");
                    exit(-1);
                }
                return 0;
            default: LOG_INVALID_TAG;
        }
    }
}

