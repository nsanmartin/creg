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

CliInput* opt_parse(Mem m[static 1], int argc, const char* argv[]) {
    int opt = -1;

    while ((opt = getopt(argc, (char **)argv, optstring)) != -1) {
       switch (opt) {
       case 'p':
           if (optind == argc) {
               return newPrintInput(m);
           } else {
               print_help(argv[0], "-p opt must be alone");
               return 0x0;
           }
       case 'h':
           print_help(argv[0], 0x0);
           return 0x0;
       case 's':
           const char* s = optarg;
           opt = getopt(argc, (char**)argv, optstring);
           if (opt != -1 || optind == argc) {
               print_help(argv[0], "-s must have query string");
               return 0x0;
           } else {
               return newRegsInputSep(m, &argv[optind], argc-optind, s);
           }
       default: /* '?' */
           print_help(argv[0],"");
           return 0x0;
       }
    }

    if (optind == argc) {
        return newStdInput(m);
    } else {
        return newRegsInput(m, &argv[optind], argc-optind);
    }
}
