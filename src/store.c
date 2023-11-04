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

bool newlineRead(const char* s) {
    while(*s) {
        if (*s++ == '\n') { return true; }
    }
    return false;
}

Err foreachReg(Err(*preFn)(const char), Err(*chunkFn)(const char*)) {
    bool ignoreStream = false;

    FILE* regfile = fopen(getRegfilePath(), "r");
    if (!regfile) {
        fprintf(stderr, "Could not read regfile %s, Aborting.", getRegfilePath());
        exit(-1);
    }

    Err e = Ok;
    char buf[regMaxSize];
    size_t regindex = 0;
    while(fgets(buf, sizeof(buf), regfile) != NULL && regindex < sizeof(_regList)) {
        bool newline_read = newlineRead(buf);
        e = preFn(_regList[regindex]);
        if (e) { return e; }
        if (newline_read) {
            regindex++;
            e = chunkFn(buf); 
            if (e) { return e; }
        } else {
            while(fgets(buf, sizeof(buf), regfile) != NULL) {
                e = chunkFn(buf); 
                if (e) { return e; }
                if (newlineRead(buf)) { regindex++; break; }
            }
        }
    }

    fclose(regfile);
    return Ok;
}


//Err foreachReg(Err(*f)(const char* ln), void(*preReg)(void)) {
//    bool ignoreStream = false;
//
//    FILE* regfile = fopen(getRegfilePath(), "r");
//    if (!regfile) {
//        fprintf(stderr, "Could not read regfile %s, Aborting.", getRegfilePath());
//        exit(-1);
//    }
//
//    char buf[regMaxSize];
//    size_t reg = 0;
//    while(fgets(buf, sizeof(buf), regfile) != NULL && reg < sizeof(_regList)) {
//        bool newline_read = newlineRead(buf);
//        if (ignoreStream) {
//            ignoreStream = !newline_read;
//        } else {
//            Err e = f(buf); 
//            if (e) {
//                fprintf(stderr, "Could process reg. Aborting.");
//                exit(-1);
//            }
//            if (!newline_read) {
//                regTooLargefn();
//                ignoreStream = true;
//            }
//        }
//    }
//
//    fclose(regfile);
//}
