#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>

#include <store.h>
#include <mem.h>
#include <cache.h>
#include <reg-string.h>


enum { regFilePathMaxLen = 1000, regBufSize = 4000 };


const char _regFileName[] = "/.reg/regfile";
const char _queryRegs[]     = "0123456789abcdefghijklmnopqrstuvwxyz";
regix_t _regindex[NRegsBound] = {0};

char _regFilePath[regFilePathMaxLen] = {0};

size_t getRegsCount() { return sizeof(_queryRegs) - 1; }

const char* getRegfilePath(void) {
    if (_regFilePath[0] == 0) {
        const char* basedir = getenv("HOME");
        if (!basedir) {
            fprintf(stderr, "Not HOME path found in env.");
            return 0x0;
        }
        size_t maxlen = regFilePathMaxLen - sizeof(_regFileName);
        size_t len = strnlen(basedir, maxlen);
        if (len == maxlen) {
            fprintf(stderr, "HOME dir path large not supported.");
            return 0x0;
        }
        strncpy(_regFilePath, basedir, len);
        size_t regfilename_size = sizeof(_regFileName);
        strncpy(&_regFilePath[len], _regFileName, regfilename_size);
    }
    return _regFilePath;
}

regix_t getRegIx(const char c){
    if (_regindex[(size_t)_queryRegs[1]] == 0) {
        memset(_regindex, -1, NRegsBound * sizeof(*_regindex));
        for (size_t ix = 0 ; _queryRegs[ix]; ++ix) {
            _regindex[(size_t)_queryRegs[ix]] = ix;
        }
    }
    if (c < 0 /* || NRegsBound <= c not needed */ ) {
        return -1;
    }
    return _regindex[(size_t)c];
}

typedef struct { bool newline; size_t ix; } LastIx;

/**
 * Asume non empty string
 */
LastIx getLastIx(const char* s) {
    size_t ix = 0;
    while(s[ix]) {
        if (s[ix] == '\n') {
            return (LastIx) { .newline=true, .ix=ix };
        }
        ++ix;
    }
    return (LastIx) { .newline=false, .ix=ix };
}

typedef struct { bool in; size_t ix; } InList;

size_t charInList(const char reg, const char* list) {
    if (!*list) { return -1; }
    size_t ix = 0;
    while(list[ix]) {
        if (reg == list[ix]) { return ix; }
        ++ix;
    }
    return ix;
}

Err ignoreUntilEol(FILE*f, bool newlineRead) {
    char buf[regBufSize];
    while(!newlineRead && fgets(buf, sizeof(buf), f) != NULL) {
        //TODO: ceck errors
        newlineRead = getLastIx(buf).newline;
    }
    return Ok;
}

/**
 * queryRegs == "" means all regs
 */
Err foreachReg(
        const char* queryRegs,
        void(*preFn)(const char),
        void(*chunkFn)(const char*, size_t len),
        void(*postFn)(void)
    )
{
    //bool ignoreStream = false;

    FILE* regfile = fopen(getRegfilePath(), "r");
    if (!regfile) {
        perror("Could not read regfile.");
        return -1;
    }

    char buf[regBufSize];
    size_t regindex = 0;

    size_t queryRegsLen = strlen(queryRegs);
    char visited[queryRegsLen];
    bzero(visited, queryRegsLen);

    while(fgets(buf, sizeof(buf), regfile) != NULL && regindex < getRegsCount()) {
        char reg = _queryRegs[regindex];
        size_t regIxInQuery = charInList(reg, queryRegs);
        bool regInList = !*queryRegs || regIxInQuery < queryRegsLen;
        LastIx lastIx = getLastIx(buf);

        if (regInList) {
            visited[regIxInQuery] = 1;
            preFn(reg);
            chunkFn(buf, lastIx.ix);
            while(!lastIx.newline && fgets(buf, sizeof(buf), regfile) != NULL) {
                lastIx = getLastIx(buf);
                chunkFn(buf, lastIx.ix);
            }
            postFn();
        } else {
            ignoreUntilEol(regfile, lastIx.newline);
        }
        regindex++;
    }
    
    
    fclose(regfile);

    for (size_t i = 0; i < queryRegsLen; ++i) {
        if (!visited[i]) {
            fprintf(stderr, "Not register %c!\n", queryRegs[i]);
            return -1;
        }
    }
    return Ok;
}


SizedBuf readFile(Mem m[static 1], FILE* f) {
    if (fseek(f, 0, SEEK_END) != 0) { 
        perror("Could not seek regfile end.");
        return (SizedBuf) { .buf=0x0, .sz=0, .e=errno};
    }

    long len = ftell(f);
    if (len < 0) { 
        perror("Could not ftell the end position of regfile.");
        return (SizedBuf) {.buf=0x0, .sz=0, .e=errno};
    }
    rewind(f);
    char* contents = memAlloc(m, len + 1);
    if (!contents) {
        perror("Could not alloc memory for regfile contents.");
        return (SizedBuf) {.buf=0x0, .sz=0, .e=errno};
    }
    size_t read = fread(contents, 1, len,f);
    //TODO: check this cast
    if (read != (size_t)len) {
        perror("Regfile could not be read.");
        return (SizedBuf) { .buf=0x0, .sz=0, .e=errno};
    }
    Err ferr = ferror(f);
    if (ferr) {
        fprintf(stderr, "There was an error reading regfile");
        return (SizedBuf) { .buf=0x0, .sz=0, .e=ferr};
    }
    contents[len] = 0;
    return (SizedBuf) { .buf=contents, .sz=len, .e=Ok};
}

Err updateRegfile(Mem m[static 1]) {
    FILE* regfile = fopen(getRegfilePath(), "r+");
    if (!regfile) {
        perror("Could not open regfile.");
        return errno;
    }
    SizedBuf regsContents = readFile(m, regfile);
    if (regsContents.e) {
        fclose(regfile);
        fprintf(stderr, "Could not read regfile.");
        return regsContents.e;
    }
    rewind(regfile);
    
    char* buf[regBufSize];
    size_t read;
    while ((read = fread(buf, 1, regBufSize, stdin))) {
        if(fwrite(buf, 1, read, regfile) < read) {
            fclose(regfile);
            perror("There was an error writing to regfile");
            return errno;
        };
    }

    Err res = Ok;
    if (ferror(regfile)
        || (fwrite(regsContents.buf, 1, regsContents.sz, regfile) < regsContents.sz) 
        || (ferror(regfile)))
    {
        perror("There was an error writing to regfile");
        res = errno;
    }

    if (EOF == fclose(regfile)) {
        perror("There was an error closing regfile");
        res = errno;
    };
    return res;
}


Err readRegsCache(RegsCache regsCache[static 1]) {
    FILE* regfile = fopen(getRegfilePath(), "r");
    if (!regfile) {
        perror("Could not read regfile.");
        return -1;
    }

    char buf[regBufSize];
    size_t regindex = 0;
    size_t offset = 0;
    regsCache->reg[regindex] = offset;

    while(fgets(buf, sizeof(buf), regfile) != NULL && regindex < sizeof(_queryRegs)) {
        LastIx lastIx = getLastIx(buf);

        if (regsCacheCopyChunk(regsCache, &offset, buf, lastIx.ix)) {
            return -1;
        };

        while(!lastIx.newline && fgets(buf, sizeof(buf), regfile) != NULL) {
            lastIx = getLastIx(buf);
            if (regsCacheCopyChunk(regsCache, &offset, buf, lastIx.ix)) {
                return -1;
            };
        }
        regindex++;
        regsCache->buf[offset++] = ' ';
        regsCache->reg[regindex] = offset;
    }
    
    fclose(regfile);
    while (++regindex < NRegsBound) {
        regsCache->reg[regindex] = offset;
    }

    return Ok;
}

Err printRegs(Mem m[static 1], const char* regs) {
    if (regs) {
        RegsCache regsCache;
        if (!initRegsCache(m, &regsCache, 8000)) {
            if (!readRegsCache(&regsCache)) {
                for (;*regs; ++regs) {
                    regix_t ix = getRegIx(*regs);
                    if (!isRegIxValid(ix)) { fprintf(stderr, "Bad registed"); }
                    else {
                        SizedBuf buf = regsCacheReg(&regsCache, ix+1);
                        if (buf.e) { fprintf(stderr, "Error reading reg"); }
                        else {
                            if (buf.sz > 1) {
                                fwrite(buf.buf, 1, buf.sz, stdout);
                            } else {
                                fprintf(stderr, "Empty ref: %c\n", *regs);
                            }
                        }
                    }
                }
                return Ok;
            } else {
                fprintf(stderr, "Could not read regs\n");
            }
        }
    }
    return -1;
}

Err printRegsSplit(Mem m[static 1], const char* regs, const char* s) {
    RegsCache regsCache;
    if (!initRegsCache(m, &regsCache, 8000)) {
        if (!readRegsCache(&regsCache)) {
            for (;*regs; ++regs) {
                regix_t ix = getRegIx(*regs);
                SizedBuf buf = regsCacheReg(&regsCache, ix);
                if (buf.e) { fprintf(stderr, "Error reading reg"); }
                else {
                    if (buf.sz > 1) {
                        StrView subs = (StrView) { .cs=s, .sz=1};
                        StrView s = (StrView) { .cs=buf.buf, .sz=buf.sz};
                        while (s.sz > 0) {
                            StrView search = findSubStrViewIx(s, subs);
                            s.sz -= search.sz + search.cs - s.cs ;
                            s.cs = search.cs + search.sz;
                            if (search.sz > 0) {
                                fwrite(search.cs, 1, search.sz, stdout);
                                fwrite("\n", 1, 1, stdout);
                            } else {
                                break;
                            }
                        }
                        
                    } else {
                        fprintf(stderr, "Empty ref: %d\n", ix);
                    }
                }
            }
            return Ok;
        } else {
            fprintf(stderr, "Could not read regs\n");
        }
    }
    return -1;
}

Err testSplit(Mem m[static 1]) {
    RegsCache regsCache;
    if (!initRegsCache(m, &regsCache, 8000)) {
        if (!readRegsCache(&regsCache)) {
            for (size_t ix = 0; ix < 10; ++ix) {
                SizedBuf buf = regsCacheReg(&regsCache, ix);
                if (buf.e) { fprintf(stderr, "Error reading reg"); }
                else {
                    if (buf.sz > 1) {
                        StrView subs = (StrView) { .cs=" ", .sz=1};
                        StrView s = (StrView) { .cs=buf.buf, .sz=buf.sz};
                        while (s.sz > 0) {
                            StrView search = findSubStrViewIx(s, subs);
                            s.sz -= search.sz + search.cs - s.cs ;
                            s.cs = search.cs + search.sz;
                            if (search.sz > 0) {
                                fwrite(search.cs, 1, search.sz, stdout);
                                fwrite("\n", 1, 1, stdout);
                            } else {
                                break;
                            }
                        }
                        
                    } else {
                        fprintf(stderr, "Empty ref: %ld\n", ix);
                    }
                }
            }
            return Ok;
        } else {
            fprintf(stderr, "Could not read regs\n");
        }
    }
    return -1;
}



