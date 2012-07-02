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


#ifndef XS_CTX_INCLUDED
#define XS_CTX_INCLUDED

#include "sock.h"
#include "mutex.h"
#include "threadpool.h"
#include "pattern_plugin.h"
#include "transport_plugin.h"


typedef struct xs_ctx
{
    /*  Array of all available socket slots. Unoccupied socket slots contain a
        NULL pointer. */
    size_t socks_num;
    struct xs_sock **socks;

    /*  Critical section wrapping the context object. */
    xs_mutex sync;

    xs_threadpool threadpool;

    /*  Plugin registries  */
    xs_patterns patterns;
    xs_transports transports;
} xs_ctx;


int xs_ctx_init (xs_ctx *self);
int xs_ctx_term (xs_ctx *self);
int xs_ctx_setopt (xs_ctx *self, int option,
                   const void *value, size_t value_len);

int xs_ctx_socket (xs_ctx *self, int type);
int xs_ctx_close (xs_ctx *self, int s);

#endif
