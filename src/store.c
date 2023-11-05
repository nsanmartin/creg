#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>

#include <store.h>
#include <mem.h>

enum { regFilePathMaxLen = 1000, regBufSize = 4000 };


const char _regFileName[] = "/.reg/regfile";
const char _queryRegs[]     = "0123456789abcdefghijklmnopqrstuvwxyz";

char _regFilePath[regFilePathMaxLen] = {0};

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
        strncpy(&_regFilePath[len], _regFileName, sizeof(_regFileName));
    }
    return _regFilePath;
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
        newlineRead = getLastIx(buf).newline;
    }
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
    bool ignoreStream = false;

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

    while(fgets(buf, sizeof(buf), regfile) != NULL && regindex < sizeof(_queryRegs)) {
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

    for (int i = 0; i < queryRegsLen; ++i) {
        if (!visited[i]) {
            fprintf(stderr, "Not register %c!\n", queryRegs[i]);
            return -1;
        }
    }
    return Ok;
}

typedef struct { char* buf; size_t sz; Err e; } SizedBuf;

SizedBuf readFile(Mem m[static 1], FILE* f) {
    if (fseek(f, 0, SEEK_END) != 0) { 
        perror("Could not seek regfile end.");
        return (SizedBuf) { .buf=0x0, .sz=0, .e=errno};
    }

    size_t len = ftell(f);
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
    if (read != len) {
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
    int read;
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


Err readRegs(
        void(*preFn)(const char),
        void(*chunkFn)(const char*, size_t len),
        void(*postFn)(void)
    )
{
    FILE* regfile = fopen(getRegfilePath(), "r");
    if (!regfile) {
        perror("Could not read regfile.");
        return -1;
    }

    char buf[regBufSize];
    size_t regindex = 0;


    while(fgets(buf, sizeof(buf), regfile) != NULL && regindex < sizeof(_queryRegs)) {
        char reg = _queryRegs[regindex];
        LastIx lastIx = getLastIx(buf);

        preFn(reg);
        chunkFn(buf, lastIx.ix);

        while(!lastIx.newline && fgets(buf, sizeof(buf), regfile) != NULL) {
            lastIx = getLastIx(buf);
            chunkFn(buf, lastIx.ix);
        }
        postFn();
        regindex++;
    }
    
    
    fclose(regfile);

    return Ok;
}
