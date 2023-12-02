#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include <regstring.h>

#define CSTRLEN(A) (sizeof(A)/sizeof(A[0]))
#define strViewFromArr(A) ((StrView){.cs=A,.sz=CSTRLEN(A)-1})

#define assertStrIsLike(CS, CSTR) assertStrViewEq(strViewFromArr(CS), CSTR)

void assertStrViewEq(StrView expected, StrView s) {
    if (expected.sz != s.sz || strncmp(expected.cs, s.cs, s.sz)) {
        fprintfmt(
            stderr, "Error: expected: '%s', result: '%s'.\n", expected.cs, s.cs
        );
    } else {
        puts("Ok");
    }
}

void assertTrue(bool p) {
    if (!p) {
        fprintfmt(stderr, "Error: expected True but was False\n");
    } else {
        puts("Ok");
    }
}

int main() {
    StrView space = (StrView) {.cs=" ", .sz=1};
    assertStrIsLike("", findNextSubStrOrLastIx("", space));

    const char* manySpaces = "                                 ";
    assertStrIsLike("", findNextSubStrOrLastIx("", space));

    const char* oneWord = "first second third";
    //assertStrIsLike("first", findNextSubStrOrLastIx(oneWord, space));

    StrView first = findNextSubStrOrLastIx(oneWord, space);
    assertStrIsLike("first", first);

    StrView second = findNextSubStrOrLastIx(first.cs+first.sz, space);
    assertStrIsLike("second", second);

    StrView third = findNextSubStrOrLastIx(second.cs+second.sz, space);
    assertStrIsLike("third", third);

    StrView end = findNextSubStrOrLastIx(third.cs+third.sz, space);
    assertTrue(end.sz == 0);

    const char* trailingSpaces = "word  ";
    StrView word = findNextSubStrOrLastIx(trailingSpaces, space);
    assertStrIsLike("word", word);

    StrView ateSpace = findNextSubStrOrLastIx(word.cs+word.sz, space);
    assertStrIsLike("", ateSpace);

    const char* strWithNewline = "before\nafter";
    StrView before = findNextSubStrOrLastIx(strWithNewline, space);
    assertStrIsLike("before", before);
    StrView newlineChar = findNextSubStrOrLastIx(before.cs+before.sz, space);
    assertStrIsLike("\n", newlineChar);
    StrView after = findNextSubStrOrLastIx(
        newlineChar.cs+newlineChar.sz, space
    );
    assertStrIsLike("after", after);
    return 0;
}
