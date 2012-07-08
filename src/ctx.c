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

#include <stdlib.h>

#include "ctx.h"
#include "err.h"
#include "likely.h"
#include "plugin.h"
#include "stdplugins.h"
#include "transport_func.h"
#include "pattern_func.h"


int xs_ctx_init (xs_ctx *self)
{
    int rc;

    self->socks_num = 512;
    self->socks = malloc (self->socks_num * sizeof (xs_sock*));
    alloc_assert (self->socks);
    memset (self->socks, 0, self->socks_num * sizeof (xs_sock *));

    rc = xs_threadlocal_init (&self->threadlocal);
    err_assert (rc);

    rc = xs_patterns_init (self);
    err_assert (rc);

    rc = xs_transports_init (self);
    err_assert (rc);

    rc = xs_add_standard_plugins (self);
    err_assert (rc);  // standard plugins should always load

    rc = xs_threadpool_init (&self->threadpool);
    err_assert (rc);

    xs_mutex_init (&self->sync);
    return 0;
}

int xs_ctx_setopt (xs_ctx *self, int option,
                   const void *value, size_t value_len) {
    switch (option) {

    case XS_IO_THREADS:
        if (value_len != sizeof(int))
            return -EINVAL;
        if (value == NULL)
            return -EFAULT;
        return xs_threadpool_resize (&self->threadpool, *(int *)value);

    default:
        return -EINVAL;

    }
}

int xs_ctx_term (xs_ctx *self)
{
    int rc;

    // Let's shutdown thread pool
    xs_mutex_lock (&self->sync);

    rc = xs_threadpool_shutdown (&self->threadpool);
    err_assert (rc);

    rc = xs_patterns_free (self);
    err_assert (rc);

    rc = xs_transports_free (self);
    err_assert (rc);

    rc = xs_threadlocal_term (&self->threadlocal);
    err_assert (rc);

    xs_mutex_unlock (&self->sync);

    xs_mutex_term (&self->sync);
    free (self->socks);
    return 0;
}

static int make_socket(xs_ctx *self, int index, int type) {
    int rc;

    xs_sock *sock = malloc (sizeof (xs_sock));
    if (unlikely (!sock))
        return -ENOMEM;

    rc = xs_sock_init (sock);
    if (unlikely (rc != 0)) {
        free (self);
        return rc;
    }
    sock->type = type;
    sock->ctx = self;
    rc = xs_init_pattern (sock);
    if (unlikely (rc != 0)) {
        xs_sock_term (sock);
        return rc;
    }

    self->socks [index] = sock;
    return index;
}

int xs_ctx_socket (xs_ctx *self, int type)
{
    int s;
    int rc;

    xs_mutex_lock (&self->sync);

    rc = xs_threadpool_ensure_ready (&self->threadpool);
    if(rc < 0)
        return rc;

    //  TODO: Find the empty slot in O(1) time!
    for (s = 0; s != self->socks_num; ++s) {
        if (!self->socks [s]) {
            s = make_socket (self, s, type);
            xs_mutex_unlock (&self->sync);
            return s;
        }
    }
    xs_mutex_unlock (&self->sync);
    return -EMFILE;
}

int xs_ctx_close (xs_ctx *self, int s)
{
    if (s > self->socks_num || !self->socks [s])
        return -EBADF;

    xs_mutex_lock (&self->sync);
    xs_sock_term (self->socks [s]);
    self->socks [s] = NULL;
    xs_mutex_unlock (&self->sync);
    return 0;
}

