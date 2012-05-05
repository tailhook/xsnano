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

#include "../include/xs.h"

#include "sub.h"
#include "err.h"
#include "likely.h"

int xs_sub_init (xs_sock *sock)
{
    int rc;
    xs_sub *self = (xs_sub*) sock;

    rc = xs_xsub_init (&self->xsub.sock);
    if (unlikely (rc < 0))
        return rc;

    /*  Initialise virtual function pointers. */
    self->xsub.sock.vfptr.term = xs_sub_term;
    self->xsub.sock.vfptr.setopt = xs_sub_setopt;
    self->xsub.sock.type = XS_SUB;

    /*  TODO  */

    return 0;
}

int xs_sub_term (xs_sock *sock)
{
    int rc;
    xs_sub *self = (xs_sub*) sock;
    
    /*  TODO  */

    rc = xs_xsub_term (&self->xsub.sock);
    if (unlikely (rc < 0))
        return rc;

    return 0;    
}

int xs_sub_setopt (xs_sock *sock, int level, int option,
    const void *optval, size_t optvallen)
{
    int rc;
    xs_sub *self = (xs_sub*) sock;

    if (level == XS_SOL_SOCKET && option == XS_SUBSCRIBE) {
        xs_assert (0);
    }

    if (level == XS_SOL_SOCKET && option == XS_UNSUBSCRIBE) {
        xs_assert (0);
    }

    return xs_sock_setopt (sock, level, option, optval, optvallen);
}
