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
}

void* memAlloc(Mem m[static 1], size_t n) {
    switch (m->tag) {
        case DumbMemTag: return malloc(n);
        case FixedSzGroupMemTag:
              if (n >= FixedSzGroupMemSz) {
                  fprintf(stderr, "%s\n", "Not enough Fixed Size Memory. Aborting.");
                  exit(-1);
              }
              m->fixedSzGroup.ptr[n] = malloc(n);
              return m->fixedSzGroup.ptr[n++];
        default: LOG_INVALID_TAG;
    }
}

Err memCleanup(Mem m[static 1]) {
    switch (m->tag) {
        case DumbMemTag: return 0;
        case FixedSzGroupMemTag:
              for (size_t i = 0; i < m->fixedSzGroup.n; ++i) { free(m->fixedSzGroup.ptr[i]); };
              return 0;
        default: LOG_INVALID_TAG;
    }
}
