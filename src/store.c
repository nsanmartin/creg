#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <store.h>

enum { regFilePathMaxLen = 1000, regMaxSize = 100 };


const char _regFileName[] = "/.reg/regfile";
const char _regList[]     = "0123456789abcdefghijklmnopqrstuvwxyz";

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

bool regInList(const char reg, const char* list) {
    if (!*list) { return true; }
    while(*list) { if (reg == *list++) { return true; } }
    return false;
}

/**
 * regList == "" means all regs
 */
Err foreachReg(
        const char* regList,
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

    char buf[regMaxSize];
    size_t regindex = 0;

    while(fgets(buf, sizeof(buf), regfile) != NULL && regindex < sizeof(_regList)) {
        char reg = _regList[regindex];
        LastIx lastIx = getLastIx(buf);
        bool reg_in_reglist = regInList(reg, regList);

        if (reg_in_reglist) {
            preFn(reg);
        }

        if (lastIx.newline) {
            regindex++;
            if (reg_in_reglist) {
                chunkFn(buf, lastIx.ix);
                postFn();
            }
        } else {
            while(fgets(buf, sizeof(buf), regfile) != NULL) {
                lastIx = getLastIx(buf);
                if (reg_in_reglist) { chunkFn(buf, lastIx.ix); }
                if (lastIx.newline) {
                    regindex++;
                    if (reg_in_reglist) { postFn(); }
                    break;
                }
            }
        }
    }

    fclose(regfile);
    return Ok;
}

