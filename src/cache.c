#include <string.h>

#include <cache.h>

Err initRegsMat(Mem m[static 1], RegsMat rc[static 1], size_t sz) {
    char* buf = memAlloc(m, sz);
    if (!buf) {
        return -1;
    }
    *rc = (RegsMat) {
        .buf=(Array){.data=buf, .sz=sz},
        .reg={0},
        .regColCount={0},
        .cols={0},
        .regMax=0
    };
    return Ok;
}

Err initRegsCache(Mem m[static 1], RegsCache rc[static 1], size_t sz) {
    char* buf = memAlloc(m, sz);
    if (!buf) {
        return -1;
    }
    *rc = (RegsCache) { .buf=buf, .bufsz=sz, .reg={0} };
    return Ok;
}

Err regsMatCopyChunk(
    RegsMat regsMat[static 1], size_t offset[static 1], const char* src, size_t n
) {
    if (*offset + n < regsMat->buf.sz) {
        memcpy(&regsMat->buf.data[*offset], src, n);
        *offset += n;
        return Ok;
    }
    fprintf(stderr, "Not enough memory for reg size (%ld)\n", regsMat->buf.sz);
    return -1;
}

Err regsCacheCopyChunk(RegsCache regsCache[static 1], size_t offset[static 1], char* src, size_t n) {
    if (*offset + n < regsCache->bufsz) {
        memcpy(&regsCache->buf[*offset], src, n);
        *offset += n;
        return Ok;
    }
    fprintf(stderr, "Not enough memory for reg size (%ld)\n", regsCache->bufsz);
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
