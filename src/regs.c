#include <string.h>

#include <regs.h>
#include <store.h>
#include <reg-string.h>

Err initRegs(Mem m[static 1], Regs rc[static 1], size_t sz) {
    char* buf = memAlloc(m, sz);
    if (!buf) {
        return -1;
    }
    *rc = (Regs) {
        .buf=(Array){.data=buf, .sz=sz},
        .reg={0},
        .ncols={0},
        .items={0},
        .nregs=0
    };
    return Ok;
}


Err regsCopyChunk(
    Regs regs[static 1], size_t offset[static 1], const char* src, size_t n
) {
    if (*offset + n < regs->buf.sz) {
        memcpy(&regs->buf.data[*offset], src, n);
        *offset += n;
        return Ok;
    }
    fprintf(stderr, "Not enough memory for reg size (%ld)\n", regs->buf.sz);
    return -1;
}


Err readRegs(Regs regs[static 1]) {
    FILE* regfile = fopen(getRegfilePath(), "r");
    if (!regfile) {
        perror("Could not read regfile.");
        return -1;
    }

    char buf[DataBufSz] = {0};
    size_t regindex = 0;
    size_t offset = 0;
    regs->reg[regindex] = offset;
    StrView sep = (StrView){.cs=" ", .sz=1};

    size_t ncols = 0;
    const size_t regscount = getRegsCount();
    while(fgets(buf, sizeof(buf), regfile) != NULL && regindex < regscount) {
        StrView next = (StrView){.cs=buf, .sz=0};
        do {
            next = findNextSubStrOrLastIx(next.cs+next.sz, sep);
            if (*next.cs == '\n') { break; }
            regs->items[ncols] = &regs->buf.data[offset];
            ncols += next.sz ? 1: 0;
            if (regsCopyChunk(regs, &offset, next.cs, next.sz)) {
                perror("could not copy reg");
                return -1;
            };
        } while (next.sz > 0);

        regs->ncols[regindex] = ncols;

        if (next.sz == 1 && *next.cs == '\n') { /* end of line */
            regindex++;
            regs->reg[regindex] = offset;
        }
    }
    
    regs->nregs = regindex;
    fclose(regfile);

    return Ok;
}

QueryResult queryRegItem(Regs r[static 1], size_t row, size_t col) {
    QueryResult res = (QueryResult){.valid=false};
    if (row < r->nregs) {
        size_t prev = row ? r->ncols[row-1] :0;
        if (col < r->ncols[row] - prev) {
            res.b  = r->items[prev + col];
            res.sz = r->items[prev + col + 1] - res.b;
            res.valid = true;
        }
    }
    return res;
}

