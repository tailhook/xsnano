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

#ifndef XS_SOCK_INCLUDED
#define XS_SOCK_INCLUDED

#include <stddef.h>
#include <pthread.h>

#include "mutex.h"
#include "pattern_plugin.h"
#include "ctx.h"
#include "msg.h"
#include "msg.h"

typedef struct xs_sock
{

    /*  Socket type (XS_PUB, XS_SUB, XS_REQ, XS_REP or similar). */
    int type;

    /*  Critical section around the whole socket object --
        except for asynchronous operations. */
    xs_mutex sync;

    /*  Context that created socket  */
    struct xs_ctx *ctx;

    /*  Pointer to pattern plugin  */
    xs_pattern_plugin *pattern;

    /*  Pointer to pattern-specific structure  */
    void *pattern_data;

} xs_sock;

int xs_sock_init (xs_sock *self);
void xs_sock_term (xs_sock *self);

int xs_sock_setopt (xs_sock *self, int level, int option, const void *optval,
    size_t optvallen);
int xs_sock_getopt (xs_sock *self, int level, int option, void *optval,
    size_t *optvallen);
int xs_sock_bind (xs_sock *self, const char *addr);
int xs_sock_connect (xs_sock *self, const char *addr);
int xs_sock_shutdown (xs_sock *self, int how);
int xs_sock_sendmsg (xs_sock *self, xs_msg *msg, int flags);
int xs_sock_recvmsg (xs_sock *self, xs_msg *msg, int flags);
int xs_socket_add_stream(void *socket, void *stream);

#endif
