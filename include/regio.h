#ifndef __REG_PRINT_H_
#define __REG_PRINT_H_



#ifndef DEBUG

#include <stdio.h>

#define file_write  fwrite
#define printfmt    printf
#define fprintfmt   fprintf
#define print_error perror

#else

static inline void noop_void(const void* p, ...) { (void)p; }
static inline size_t noop_size_t(const void* p, ...) { (void)p; return 1; }

#include <stdio.h>

#define file_write  noop_size_t
#define printfmt    noop_void
#define fprintfmt   noop_void
#define print_error noop_void
#endif

#endif
