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

#include "sock.h"
#include "err.h"

int xs_sock_init (xs_sock *self, int type)
{
    return -ENOTSUP;
}

int xs_sock_term (xs_sock *self)
{
    return -ENOTSUP;
}

int xs_sock_setopt (xs_sock *self, int level, int option, const void *optval,
    size_t optvallen)
{
    return -ENOTSUP;
}

int xs_sock_getopt (xs_sock *self, int level, int option, void *optval,
    size_t *optvallen)
{
    return -ENOTSUP;
}

int xs_sock_bind (xs_sock *self, const char *addr)
{
    return -ENOTSUP;
}

int xs_sock_connect (xs_sock *self, const char *addr)
{
    return -ENOTSUP;
}

int xs_sock_shutdown (xs_sock *self, int how)
{
    return -ENOTSUP;
}

int xs_sock_send (xs_sock *self, const void *buf, size_t len, int flags)
{
    return -ENOTSUP;
}

int xs_sock_recv (xs_sock *self, void *buf, size_t len, int flags)
{
    return -ENOTSUP;
}

