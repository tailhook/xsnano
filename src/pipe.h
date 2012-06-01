/*
    Copyright (c) 2012 Paul Colomiets

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

#ifndef XS_PIPE_INCLUDED
#define XS_PIPE_INCLUDED

#include <sys/queue.h>

#define XS_DECLARE_PIPE(type, granularity, pipetype) \
    struct xs_##pipetype##_chunk { \
        TAILQ_ENTRY(xs_##pipetype##_chunk) list; \
        type data[granularity]; \
    } xs_##pipetype##_chunk; \
    struct { \
        TAILQ_HEAD(xs_##pipetype##_list, xs_##pipetype##_chunk) chunks;\
        int begin_pos; \
        int end_pos; \
    } pipetype; \
    inline int xs_##pipetype##_init(pipetype

#define XS_PIPE_INIT(pipe) do {\
    TAILQ_INIT(&(pipe)->chunks); \
    (pipe)->begin_pos = 0; \
    (pipe)->end_pos = 0; \
    } while (0); \

XS_DECLARE_PIPE(int, 16, int_pipe);


#endif // XS_PIPE_INCLUDED

