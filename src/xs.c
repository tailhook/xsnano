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

#include <string.h>

#include "../include/xs.h"
#include "err.h"
#include "ctx.h"
#include "likely.h"

#define XS_CHECK_SOCKET(s) \
    if (s > ctx.socks_num || !ctx.socks [s]) {\
        errno = EBADF;\
        return -1;\
    }

/*  Global state of the library. */
static xs_ctx ctx;

int xs_init (void)
{
    int rc;

    rc = xs_ctx_init (&ctx);
    if (unlikely (rc < 0)) {
        errno = -rc;
        return -1;
    }
    return 0;
}

int xs_term (void)
{
    int rc;

    rc = xs_ctx_term (&ctx);
    if (unlikely (rc < 0)) {
        errno = -rc;
        return -1;
    }
    return 0;
}

int xs_setctxopt (int option, const void *value, size_t value_len)
{
    int rc;

    rc = xs_ctx_setopt (&ctx, option, value, value_len);
    if (unlikely (rc < 0)) {
        errno = -rc;
        return -1;
    }
    return 0;
}

int xs_socket (int type)
{
    int s;

    s = xs_ctx_socket (&ctx, type);
    if (unlikely (s < 0)) {
        errno = -s;
        return -1;
    }
    return s;

}

int xs_close (int s)
{
    int rc;

    rc = xs_ctx_close (&ctx, s);
    if (unlikely (rc < 0)) {
        errno = -rc;
        return -1;
    }
    return 0;
}

int xs_setsockopt (int s, int level, int option, const void *optval,
    size_t optvallen)
{
    int rc;

    XS_CHECK_SOCKET (s);
    rc = xs_sock_setopt (ctx.socks [s], level, option, optval, optvallen);
    if (unlikely (rc < 0)) {
        errno = -rc;
        return -1;
    }
    return 0;
}

int xs_getsockopt (int s, int level, int option, void *optval,
    size_t *optvallen)
{
    int rc;

    XS_CHECK_SOCKET (s);
    rc = xs_sock_getopt (ctx.socks [s], level, option, optval, optvallen);
    if (unlikely (rc < 0)) {
        errno = -rc;
        return -1;
    }
    return 0;
}

int xs_bind (int s, const char *addr)
{
    int rc;

    XS_CHECK_SOCKET (s);
    rc = xs_sock_bind (ctx.socks [s], addr);
    if (unlikely (rc < 0)) {
        errno = -rc;
        return -1;
    }
    return rc;
}

int xs_connect (int s, const char *addr)
{
    int rc;

    XS_CHECK_SOCKET (s);
    rc = xs_sock_connect (ctx.socks [s], addr);
    if (unlikely (rc < 0)) {
        errno = -rc;
        return -1;
    }
    return rc;
}

int xs_shutdown (int s, int how)
{
    int rc;

    XS_CHECK_SOCKET (s);
    rc = xs_sock_shutdown (ctx.socks [s], how);
    if (unlikely (rc < 0)) {
        errno = -rc;
        return -1;
    }
    return 0;
}

int xs_send (int s, const void *buf, size_t len, int flags)
{
    int rc;

    XS_CHECK_SOCKET (s);
    xs_msg msg;
    rc = xs_msg_init(&msg, len);
    if (unlikely (rc < 0)) {
        errno = -rc;
        return -1;
    }
    memcpy(xs_msg_data(&msg), buf, len);
    rc = xs_sock_sendmsg (ctx.socks [s], &msg, flags);
    if (unlikely (rc < 0)) {
        xs_msg_term(&msg);
        errno = -rc;
        return -1;
    }
    return rc;
}

int xs_recv (int s, void *buf, size_t len, int flags)
{
    int rc;

    XS_CHECK_SOCKET (s);
    xs_msg msg;
    rc = xs_msg_init(&msg, 0);
    if (unlikely (rc < 0)) {
        errno = -rc;
        return -1;
    }
    rc = xs_sock_recvmsg (ctx.socks [s], &msg, flags);
    if (unlikely (rc < 0)) {
        errno = -rc;
        return -1;
    }
    if(len > rc) {
        len = rc;
    }
    memcpy(buf, xs_msg_data(&msg), len);
    xs_msg_term (&msg);
    return rc;
}

void xs_version (int *major, int *minor, int *patch)
{
    if (major)
        *major = XS_VERSION_MAJOR;
    if (minor)
        *minor = XS_VERSION_MINOR;
    if (patch)
        *patch = XS_VERSION_PATCH;
}

int xs_errno (void)
{
    return errno;
}

const char *xs_strerror (int errnum)
{
    return xs_err_strerror (errnum);
}

