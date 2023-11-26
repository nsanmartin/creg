#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <mem.h>
#include <opt-parse.h>

const char* optstring = "s:ph";

CliInput* newHelpInput(Mem m[static 1]) {
    CliInput* rv = memAlloc(m, sizeof(*rv));
    if (!rv) { return 0x0;}
    *rv = (CliInput) { .tag=HelpInputTag };
    return rv;
}

CliInput* newPrintInput(Mem m[static 1]) {
    CliInput* rv = memAlloc(m, sizeof(*rv));
    if (!rv) { return 0x0;}
    *rv = (CliInput) { .tag=PrintInputTag };
    return rv;
}

CliInput* newStdinInput(Mem m[static 1]) {
    CliInput* rv = memAlloc(m, sizeof(*rv));
    if (!rv) { return 0x0;}
    *rv = (CliInput) { .tag=StdinInputTag };
    return rv;
}

CliInput* newQueriesInput(Mem m[static 1], const char** q, size_t n) {
    CliInput* rv = memAlloc(m, sizeof(*rv));
    if (!rv) { return 0x0;}
    *rv = (CliInput) {
        .tag=QueriesInputTag,
        .queries=(QueriesInput) {
            .queries=q,
            .n=n
        }
    };
    return rv;
}

CliInput* newQueriesSepInput(Mem m[static 1], const char** q, size_t n, const char* s) {
    CliInput* rv = memAlloc(m, sizeof(*rv));
    if (!rv) { return 0x0;}
    *rv = (CliInput) {
        .tag=QueriesSepInputTag,
        .queriesSep=(QueriesSepInput) {
            .queries=q,
            .n=n,
            .sep=s
        }
    };
    return rv;
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
        if (*q == '.') {
           if (dotRead++) { return false; }
        }
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

bool isAllQueriesSepInput(int argc, const char* argv[], int* sepIndex) {
    for (int i = 1; i < argc; ++i) {
        if (strcmp("-s", argv[i]) == 0) {
            if (*sepIndex) { return false; }
            *sepIndex = i;
        }
        else if (!isQuery(argv[i])) { return false; }
    }
    return argc > 3 && argc > *sepIndex + 1;
}

bool isStdInInput(int argc, const char* argv[]) {
    (void)argv;
    return argc == 1;
}

CliInput* opt_parse(Mem m[static 1], int argc, const char* argv[]) {
    if (isPrintInput(argc, argv)) { return newPrintInput(m); }
    else if (isStdInInput(argc, argv)) { return newStdinInput(m); }
    else if (isAllQueriesInput(argc, argv)) { return newQueriesInput(m, argv + 1, argc-1); }
    else if (isHelpInput(argc, argv)) { return newHelpInput(m); }
    else { 
        int sepIndex = 0;
        if (isAllQueriesSepInput(argc, argv, &sepIndex)) {
            const char* s = argv[sepIndex];
            argv[sepIndex] = argv[1];
            s = argv[sepIndex+1];
            argv[sepIndex+1] = argv[2];
            return newQueriesSepInput(m, argv + 3, argc-3, s);
        }
    }

    fprintf(stderr, "Bad input\n");
    return 0x0;
}

