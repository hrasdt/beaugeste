#ifndef MACROS_H

#include <stdio.h>
#include <assert.h>

#define min(a, b) (a < b)? (a) : (b)
#define max(a, b) (a > b)? (a) : (b)

// If something isn't already null, unreference it and NULL it out.
// This is particularly useful with APIs such as lib{input,udev}.
#define COND_UNREF(var, fn) \
    if (NULL != var){       \
        fn(var);            \
        var = NULL;         \
    }

// Disable "unused variable" warnings for those void*user_data parameters.
// We don't use those at the moment.
#define UNUSED(var)         \
    (void)var

// Various macros for printing messages. Later on we'll be able to switch which
// of these are displayed with some sort of flag; until then, let's just
// pretend they serve different purposes.
#define DBG(...) fprintf(stderr, "debug: \t" __VA_ARGS__ ); fprintf(stderr, "\n")
#define WRN(...) fprintf(stderr, "warning: \t" __VA_ARGS__ ); fprintf(stderr, "\n")
#define ERR(...) fprintf(stderr, "error: \t" __VA_ARGS__ ); fprintf(stderr, "\n")

#define MACROS_H
#endif
