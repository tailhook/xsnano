/*
    Copyright (c) 2012 250bpm s.r.o.

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.
*/

#ifndef XS_ERR_INCLUDED
#define XS_ERR_INCLUDED

#include <errno.h>
#include <stdio.h>
#include <string.h>

/*  Include XS header to define XS-specific error codes. */
#include "../include/xs.h"

#include "likely.h"

/*  Same as system assert(). However, under Win32 assert has some deficiencies.
    Thus this macro. */
#define xs_assert(x) \
    do {\
        if (unlikely (!(x))) {\
            fprintf (stderr, "Assertion failed: %s (%s:%d)\n", #x, \
                __FILE__, __LINE__);\
            xs_err_abort ();\
        }\
    } while (0)

/*  Checks the error code, assuming the negative numbers represent errors. */
#define err_assert(x) \
    do {\
        if (unlikely (x < 0)) {\
            fprintf (stderr, "%s (%s:%d)\n", xs_err_strerror (-(x)),\
                __FILE__, __LINE__);\
            xs_err_abort ();\
        }\
    } while (0)


/*  Checks whether memory allocation was successful. */
#define alloc_assert(x) \
    do {\
        if (unlikely (!x)) {\
            fprintf (stderr, "Out of memory (%s:%d)\n",\
                __FILE__, __LINE__);\
            xs_err_abort ();\
        }\
    } while (0)

/*  Check the condition. If false prints out the errno. */
#define errno_assert(x) \
    do {\
        if (unlikely (!(x))) {\
            fprintf (stderr, "%s (%s:%d)\n", strerror (errno),\
                __FILE__, __LINE__);\
            xs_err_abort ();\
        }\
    } while (0)

/*  Checks whether supplied errno number is an error. */
#define errnum_assert(x) \
    do {\
        if (unlikely (x)) {\
            fprintf (stderr, "%s (%s:%d)\n", xs_err_strerror (x), \
                __FILE__, __LINE__);\
            xs_err_abort ();\
        }\
    } while (0)

void xs_err_abort (void);
const char *xs_err_strerror (int errnum);

#endif
