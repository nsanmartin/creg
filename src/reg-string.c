#include <string.h>

#include <reg-string.h>

// return the index of first occurence od substr, or end if not found
StrView findSubStrViewIx(StrView s, StrView sep) {
    StrView res = (StrView) { .cs=s.cs, .sz=s.sz};
    size_t beg = 0;
    if (sep.sz < s.sz) {
        size_t i = 0;

        while (i < s.sz - sep.sz && (strncmp(s.cs+i,sep.cs,sep.sz) == 0)) { i += sep.sz; }
        res.cs += i; 
        res.sz -= i;
        beg = i;
        ++i;
        for (; i < s.sz - sep.sz; ++i) {
            if (strncmp(s.cs+i,sep.cs,sep.sz) == 0) {
                res.sz = i - beg;
                break;
            }
        }
    }
    return res;
}

size_t findSubStrViewIx0(StrView s, StrView subs) {
    if (subs.sz < s.sz) {
        size_t i = 0;

        while (i < s.sz - subs.sz && (strncmp(s.cs+i,subs.cs,subs.sz) == 0)) { i += subs.sz; }
        ++i;
        for (; i < s.sz - subs.sz; ++i) {
            if (strncmp(s.cs+i,subs.cs,subs.sz) == 0) {
                return i;
            }
        }
    }
    return s.sz;
}


size_t findSubStrIx(const char* s, const char* subs) {
    size_t i = 0;
    for (;  s[i]; ++i) {
        size_t len = 0;
        for (; subs[len] && s[i+len] && s[i+len] == subs[len]; )
            ;
        if (!subs[len]) { break; }
    }
    return i;
}
