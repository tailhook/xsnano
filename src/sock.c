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
#include <string.h>

#include "../include/xs.h"

#include "sock.h"
#include "err.h"
#include "likely.h"
#include "xpub.h"

int xs_sock_alloc (xs_sock **self, int type)
{
    int rc;

    if (unlikely (!self))
        return -EFAULT;

    switch (type) {
    case XS_XPUB:
        *self = malloc (sizeof (xs_xpub));
        alloc_assert (*self);
        rc = xs_xpub_init ((xs_xpub*) *self);
        break;
    default:
        return -EINVAL;
    }
    if (unlikely (rc < 0)) {
        free (*self);
        *self = NULL;
        return rc;
    }
    return 0;
}

int xs_sock_dealloc (xs_sock *self)
{
    int rc;

    rc = self->vfptr.term (self);
    if (unlikely (rc < 0))
        return rc;
    free (self);
    return 0;
}

int xs_sock_init (xs_sock *self)
{
    /*  Set virtual members to invalid values to chatch any potential errors. */
    memset (&self->vfptr, 0, sizeof (self->vfptr));
    self->type = -1;

    xs_mutex_init (&self->sync);
}

int xs_sock_term (xs_sock *self)
{
    xs_mutex_term (&self->sync);
}

int xs_sock_setopt (xs_sock *self, int level, int option, const void *optval,
    size_t optvallen)
{
    return -ENOTSUP;
}

int xs_sock_getopt (xs_sock *self, int level, int option, void *optval,
    size_t *optvallen)
{
    /*  Check for invalid pointers. */
    if (!optval || !optvallen)
        return -EFAULT;

    /*  TODO: Other levels should be implemented by spefic socket types. */
    if (level != XS_SOL_SOCKET)
        return -EINVAL;

    /*  Find the option specified and return its value. */
    switch (option) {
    case XS_TYPE:
        if (*optvallen < sizeof (int))
            return -EINVAL;
        *((int*) optval) = self->type;
        *optvallen = sizeof (int);
        return 0;
    }

    /*  The option specified does not exist on this level. */
    return -ENOPROTOOPT;
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

