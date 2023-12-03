#include <stdlib.h>

#include <mem.h>

Err initMem(Mem m[static 1]) {
    switch (m->tag) {
        case DumbMemTag: return 0;
        case FixedSzGroupMemTag:
              m->fixedSzGroup.n = 0;
              return 0;
        default: LOG_INVALID_TAG;
    }
    return -1;
}

void* memAlloc(Mem m[static 1], size_t n) {
    char* buf = 0x0;
    switch (m->tag) {
        case DumbMemTag:
            buf = malloc(n);
            if (!buf) {
                print_error("Could nor obtain memory");
            }
            return buf;
        case FixedSzGroupMemTag:
              if (n >= FixedSzGroupMemSz) {
                  fprintfmt(stderr, "%s\n", "Not enough Fixed Size Memory. Aborting.");
                  return 0x0;
              }
              buf = malloc(n);
              if (!buf) {
                  print_error("Could nor obtain memory");
                  return 0x0;
              }
              return m->fixedSzGroup.ptr[n++] = buf;
        default: LOG_INVALID_TAG;
    }
    return 0x0;
}

Err memCleanup(Mem m[static 1]) {
    switch (m->tag) {
        case DumbMemTag: return 0;
        case FixedSzGroupMemTag:
              for (size_t i = 0; i < m->fixedSzGroup.n; ++i) { free(m->fixedSzGroup.ptr[i]); };
              return 0;
        default: LOG_INVALID_TAG;
    }
    return -1;
}
