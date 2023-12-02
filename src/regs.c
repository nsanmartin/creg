#include <ctype.h>
#include <string.h>

#include <regs.h>
#include <store.h>
#include <regstring.h>


enum { ReadRegsDataBufSz = 4000 };

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


Err regsCopyChunk(Regs regs[static 1], size_t offset[static 1], const char* src, size_t n) {
    if (*offset + n < regs->buf.sz) {
        memcpy(&regs->buf.data[*offset], src, n);
        *offset += n;
        return Ok;
    }
    //TODO: use dynamic memory and realloc if we need more memory.
    fprintf(stderr, "Not enough memory for reg size (%ld)\n", regs->buf.sz);
    return -1;
}


Err readRegs(Regs regs[static 1], const StrView sep) {
    FILE* regfile = fopen(getRegfilePath(), "r");
    Err e = !regfile;
    if (e) {
        perror("Could not read regfile.");
        return -1;
    }

    char buf[ReadRegsDataBufSz] = {0};
    size_t regindex = 0;
    size_t offset = 0;
    regs->reg[regindex] = offset;

    size_t ncols = 0;
    const size_t regscount = getRegsCount();

    while(fgets(buf, sizeof(buf), regfile) != NULL && regindex < regscount) {
        StrView next = (StrView){.cs=buf, .sz=0};
        do {
            next = findNextSubStrOrLastIx(next.cs+next.sz, sep);
            if (*next.cs == '\n') { break; }
            regs->items[ncols] = &regs->buf.data[offset];
            ncols += next.sz ? 1: 0;
            e = regsCopyChunk(regs, &offset, next.cs, next.sz);
            if (e) {
                return e;
            };
        } while (next.sz > 0);

        regs->ncols[regindex] = ncols;

        if (next.sz == 1 && *next.cs == '\n') { /* end of line */
            regindex++;
            regs->reg[regindex] = offset;
        }
    }
    
    regs->items[ncols] = &regs->buf.data[offset];
    regs->nregs = regindex;
    fclose(regfile);

    return e;
}

QueryResult queryReg(const Regs r[static 1], regix_t row) {
    QueryResult res = (QueryResult){.valid=false};
    if (0 <= row && row < r->nregs) {
        size_t prev = row ? r->ncols[row-1] :0;
        size_t next = r->ncols[row];
        res.b  = r->items[prev];
        res.sz = r->items[next] - res.b;
        res.valid = true;

    }
    return res;
}

QueryResult queryRegItem(const Regs r[static 1], regix_t row, size_t col) {
    QueryResult res = (QueryResult){.valid=false};
    if (0 <= row && row < r->nregs) {
        size_t prev = row ? r->ncols[row-1] :0;
        if (col < r->ncols[row] - prev) {
            res.b  = r->items[prev + col];
            res.sz = r->items[prev + col + 1] - res.b;
            res.valid = true;
        }
    }
    return res;
}

/**
 * Fills rs and cs with que query data in q.
 * rs contains all the registes (or rows) expresed in RegIx
 * cs contains all the columns expressed in size_t
 */
Err fillQuery(const char* q, ArraySizeT* rs, ArraySizeT* cs) {
    if (!q) { return -1; }
    size_t reg = 0;
    for(; *q; ++q) {
        if (*q == '.') { break; }
        if (reg >= rs->sz) { return -1; }
        rs->data[reg++] = getRegIx(*q);
    }
    rs->sz = reg;
    
    if (!q) { return -1; }
    if (*q == '\0') {
        cs->sz = 0;
        return Ok;
    }
    //skip dot
    if (!*q++ || !*q) { return -1; }

    size_t col = 0;
    for(; *q; ++q) {
        if (col >= cs->sz) {
            cs->data[col++] = BadCol;
        } else if (isdigit(*q)) {
            cs->data[col++] = *q - '0';
        } else if (islower(*q)) {
            cs->data[col++] = *q - 'a' + '9';
        } else {
            return -1;
        }
    }
    cs->sz = col;
    return Ok;
}

void printQueryIfValid(QueryResult* qr, StrView sep) {
    if (qr->valid) {
        fwrite(qr->b, 1, qr->sz, stdout);
        fwrite(sep.cs, 1, sep.sz, stdout);
    } else {
        fprintf(stderr, "nternal error: invalid query\n");
    }
}

Err printQuery(const Regs r[static 1], const char* q) {
    size_t rbuf[100];
    size_t cbuf[100];
    ArraySizeT rs = {.data=rbuf, .sz=100};
    ArraySizeT cs = {.data=cbuf, .sz=100};

    Err e = fillQuery(q, &rs, &cs);
    if (e) { return e; }

    StrView sep = {.cs=" ", .sz=1};

    for (size_t i = 0; i < rs.sz; ++i) {
        regix_t regix = rs.data[i];

        if (regOutOfRange(r, regix)) {
            fprintf(stderr, "invalid register in query\n");
            break;
        }

        if (cs.sz == 0) { /* no dot */
            size_t ncols = colsInReg(r, regix);
            for (size_t j = 0; j < ncols; ++j) {
                QueryResult qr = queryRegItem(r, regix, j);
                printQueryIfValid(&qr, sep);
            }
        }

        /* cs.sz > 0 so query has dot (eg 01.23) */
        for (size_t j = 0; j < cs.sz; ++j) {
            colix_t col = cs.data[j];
            if (col == BadCol || colOutOfRange(r, regix, col)) {
                fprintf(stderr, "invalid column in query\n");
            } else {
                QueryResult qr = queryRegItem(r, regix, col);
                printQueryIfValid(&qr, sep);
            }
        }
    }
    return e;
}
