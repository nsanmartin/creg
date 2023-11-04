#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <store.h>

enum { regFilePathMaxLen = 1000, regBufSize = 100 };


const char _regFileName[] = "/.reg/regfile";
const char _queryRegs[]     = "0123456789abcdefghijklmnopqrstuvwxyz";

char _regFilePath[regFilePathMaxLen] = {0};

const char* getRegfilePath(void) {
    if (_regFilePath[0] == 0) {
        const char* basedir = getenv("HOME");
        if (!basedir) {
            fprintf(stderr, "%s\n", "Not HOME path found in env, aborting.");
            exit(-1);
        }
        size_t maxlen = regFilePathMaxLen - sizeof(_regFileName);
        size_t len = strnlen(basedir, maxlen);
        if (len == maxlen) {
            fprintf(stderr, "%s Aborting.\n", "HOME dir path large not supporte.");
            exit(-1);
        }
        strncpy(_regFilePath, basedir, len);
        strncpy(&_regFilePath[len], _regFileName, sizeof(_regFileName));
    }
    return _regFilePath;
}

//bool newlineRead(const char* s) {
//    while(*s) {
//        if (*s++ == '\n') { return true; }
//    }
//    return false;
//}

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
        fprintf(stderr, "Could not read regfile %s, Aborting.", getRegfilePath());
        exit(-1);
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
            if (!lastIx.newline) {
                while(fgets(buf, sizeof(buf), regfile) != NULL) {
                    lastIx = getLastIx(buf);
                    chunkFn(buf, lastIx.ix);
                    if (lastIx.newline) {
                        if (regInList) { postFn(); }
                        break;
                    }
                }
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

