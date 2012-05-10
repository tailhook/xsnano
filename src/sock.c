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

/* TODO: Remove */
#include <sys/types.h>
#include <sys/socket.h>

#include "../include/xs.h"

#include "sock.h"
#include "err.h"
#include "likely.h"

#include "pub.h"
#include "sub.h"
#include "xpub.h"
#include "xsub.h"

void xs_sock_send_done (void *arg)
{
    int rc;
    xs_sock *self = (xs_sock*) arg;

    xs_mutex_lock (&self->sync);

    /*  TODO: Mark the outstream as readable. */

    /*  Release the threads waiting to send. */
    rc = pthread_cond_broadcast (&self->writeable);
    errnum_assert (rc);

    xs_mutex_unlock (&self->sync);
}

int xs_sock_alloc (xs_sock **self, int type)
{
    int rc;

    if (unlikely (!self))
        return -EFAULT;

    switch (type) {
    case XS_PUB:
        *self = malloc (sizeof (xs_pub));
        alloc_assert (*self);
        rc = xs_pub_init (*self);
        break;
    case XS_SUB:
        *self = malloc (sizeof (xs_sub));
        alloc_assert (*self);
        rc = xs_sub_init (*self);
        break;
    case XS_XPUB:
        *self = malloc (sizeof (xs_xpub));
        alloc_assert (*self);
        rc = xs_xpub_init (*self);
        break;
    case XS_XSUB:
        *self = malloc (sizeof (xs_xsub));
        alloc_assert (*self);
        rc = xs_xsub_init (*self);
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

void xs_sock_dealloc (xs_sock *self)
{
    self->vfptr.term (self);
    free (self);
}

int xs_sock_init (xs_sock *self)
{
    int rc;

    /*  Set virtual members to invalid values to catch any potential errors. */
    memset (&self->vfptr, 0, sizeof (self->vfptr));
    self->vfptr.term = xs_sock_term;
    self->vfptr.setopt = xs_sock_setopt;
    self->vfptr.getopt = xs_sock_getopt;
    self->type = -1;

    xs_mutex_init (&self->sync);
    rc = pthread_cond_init (&self->writeable, NULL);
    errnum_assert (rc);
    rc = pthread_cond_init (&self->readable, NULL);
    errnum_assert (rc);

int sv [2];
rc = socketpair (AF_UNIX, SOCK_STREAM, 0, sv);

    rc = xs_outstream_init (&self->out, sv [0], xs_sock_send_done, self);
    err_assert (rc);
}

void xs_sock_term (xs_sock *self)
{
    int rc;

    xs_outstream_term (&self->out);
    rc = pthread_cond_destroy (&self->readable);
    errnum_assert (rc);
    rc = pthread_cond_destroy (&self->writeable);
    errnum_assert (rc);
    xs_mutex_term (&self->sync);
}

int xs_sock_setopt (xs_sock *self, int level, int option, const void *optval,
    size_t optvallen)
{
    /*  Check for invalid pointers. */
    if (!optval)
        return -EFAULT;

    /*  As for now, no generic options are supported. */
    return -EINVAL;
}

int xs_sock_getopt (xs_sock *self, int level, int option, void *optval,
    size_t *optvallen)
{
    /*  Check for invalid pointers. */
    if (!optval || !optvallen)
        return -EFAULT;

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
    int rc;

    /*  Convert buffer into a message. */
    xs_msg msg;
    rc = xs_msg_init (&msg, len);
    if (rc < 0)
        return rc;
    memcpy (xs_msg_data (&msg), buf, len);

    xs_mutex_lock (&self->sync);
    while (1) {

        /*  Start the send operation. */
        rc = xs_outstream_send (&self->out, &msg);

        /*  If send succeeded synchronously, we can return immediately. */
        if (likely (rc == 0)) {
            xs_mutex_unlock (&self->sync);
            return (int) len;
        }

        /*  If the asynchronous send operation is pending, we can return
            immediately, however, we have to mark the connection is not
            writeable at the moment. */
        if (rc == -EINPROGRESS) {
            xs_mutex_unlock (&self->sync);
            return (int) len;
        }

        /*  In send is non-blocking and we cannot send, return to the caller. */
        xs_assert (rc == -EAGAIN);
        if (flags & XS_DONTWAIT) {
            xs_mutex_unlock (&self->sync);
            return -EAGAIN;
        }

        /*  Wait till socket becomes writeable. */
        rc = pthread_cond_wait (&self->writeable, &self->sync);
        errnum_assert (rc);
    }
}

int xs_sock_recv (xs_sock *self, void *buf, size_t len, int flags)
{
    return -ENOTSUP;
}

