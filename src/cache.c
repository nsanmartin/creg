#include <cache.h>

Err initRegsCache(Mem m[static 1], RegsCache rc[static 1], size_t sz) {
    char* buf = memAlloc(m, sz);
    if (!buf) {
        return -1;
    }
    *rc = (RegsCache) { .buf=buf, .bufsz=sz, .reg={0} };
    return Ok;
}
