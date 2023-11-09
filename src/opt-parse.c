#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <mem.h>
#include <opt-parse.h>

const char* optstring = "s:ph";

CliInput* newPrintInput(Mem m[static 1]) {
    CliInput* rv = memAlloc(m, sizeof(*rv));
    if (!rv) { return 0x0;}
    *rv = (CliInput) { .tag=PrintInputTag };
    return rv;
}

CliInput* newStdInput(Mem m[static 1]) {
    CliInput* rv = memAlloc(m, sizeof(*rv));
    if (!rv) { return 0x0;}
    *rv = (CliInput) { .tag=StdInputTag };
    return rv;
}

CliInput* newRegsInput(Mem m[static 1], const char** q, size_t n) {
    CliInput* rv = memAlloc(m, sizeof(*rv));
    if (!rv) { return 0x0;}
    *rv = (CliInput) {
        .tag=RegsInputTag,
        .regs=(RegsInput) {
            .regs=q,
            .n=n
        }
    };
    return rv;
}

CliInput* newRegsInputSep(Mem m[static 1], const char** q, size_t n, const char* s) {
    CliInput* rv = memAlloc(m, sizeof(*rv));
    if (!rv) { return 0x0;}
    *rv = (CliInput) {
        .tag=RegsInputSepTag,
        .regsSep=(RegsInputSep) {
            .regs=q,
            .n=n,
            .sep=s
        }
    };
    return rv;
}

void print_help(const char* prog, const char* err_msg) {
    if (err_msg) {
        fprintf(stderr,"%s\n", err_msg);
    }
    fprintf(stdout, "usage: %s [-psh] [0-9a-zA-z]\n", prog);
}

bool isPrintInput(int argc, const char* argv[]) { // just: -p
    return argc == 2 && strcmp("-p", argv[1]) == 0;
}

bool isHelpInput(int argc, const char* argv[]) { // just: -h
    return argc == 2 && strcmp("-h", argv[1]) == 0;
}

bool isQuery(const char* q) {
    int dotRead = 0;
    if (!q) { return false; }

    for (; *q; ++q) {
        if (*q == '.' && dotRead++) { return false; }
        else if (!isalnum(*q)) { return false; }
    }
    return true;
}

bool isAllQueriesInput(int argc, const char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        if (!isQuery(argv[i])) { return false; }
    }
    return argc > 1;
}

bool isAllQueriesSepInput(int argc, const char* argv[]) {
    int sepRead = 0;
    for (int i = 1; i < argc; ++i) {
        if (strcmp("-s", argv[i]) == 0 && sepRead++) { return false; }
        else if (!isQuery(argv[i])) { return false; }
    }
    return argc > 2;
}

bool isStdInInput(int argc, const char* argv[]) {
    return argc == 1;
}

CliInput* opt_parse(Mem m[static 1], int argc, const char* argv[]) {
    if (isPrintInput(argc, argv)) { return newPrintInput(m); }
    else if (isStdInInput(argc, argv)) { return newStdInput(m); }
    else if (isAllQueriesInput(argc, argv)) { return newRegsInput(m, argv + 1, argc); }
    else if (isHelpInput(argc, argv)) {
        print_help(argv[0], "");
        return 0x0;
    }
    else if (isAllQueriesSepInput(argc, argv)) {
        fprintf(stderr, "Not implemented\n");
        return 0x0;
        ///return newRegsInputSep(m, argv + 1, argc);
    }

    fprintf(stderr, "Bad input\n");
    return 0x0;
}

