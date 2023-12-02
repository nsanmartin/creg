#include <string.h>

#include <cache.h>


Err initRegsCache(Mem m[static 1], RegsCache rc[static 1], size_t sz) {
    char* buf = memAlloc(m, sz);
    if (!buf) {
        return -1;
    }
    *rc = (RegsCache) { .buf=buf, .bufsz=sz, .reg={0} };
    return Ok;
}

Err regsCacheCopyChunk(RegsCache regsCache[static 1], size_t offset[static 1], char* src, size_t n) {
    if (*offset + n < regsCache->bufsz) {
        memcpy(&regsCache->buf[*offset], src, n);
        *offset += n;
        return Ok;
    }
    fprintfmt(stderr, "Not enough memory for reg size (%ld)\n", regsCache->bufsz);
    return -1;
}

SizedBuf regsCacheReg(RegsCache regsCache[static 1], size_t regindex) {
    SizedBuf buf = (SizedBuf) {.e=-1};
    if (regindex < NRegsBound-1) {
        size_t offset = regsCache->reg[regindex];
        buf = (SizedBuf) {
            .buf=regsCache->buf + offset,
            .sz=regsCache->reg[regindex+1] - offset,
            .e=Ok
        };
    }
    return buf;
}
