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
#include "msg.h"
#include "stream.h"
#include "transport_func.h"


int xs_sock_init (xs_sock *self)
{
    xs_mutex_init (&self->sync);
    return 0;
}

void xs_sock_term (xs_sock *self)
{
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
    xs_transport *trans;
    int rc = xs_create_transport (self->ctx, &trans, addr);
    if (rc < 0)
        return rc;
    trans->socket = self;
    rc = trans->plugin->bind (trans, addr);
    if (rc < 0) {
        free(trans);
        return rc;
    }

    return 0;
}

int xs_sock_connect (xs_sock *self, const char *addr)
{
    xs_transport *trans;
    int rc = xs_create_transport (self->ctx, &trans, addr);
    if (rc < 0)
        return rc;
    trans->socket = self;
    rc = trans->plugin->connect (trans, addr);
    if (rc < 0) {
        free(trans);
        return rc;
    }
    return 0;
}

int xs_sock_shutdown (xs_sock *self, int how)
{
    return -ENOTSUP;
}

int xs_sock_sendmsg (xs_sock *self, xs_msg *msg, int flags)
{
    return self->pattern->send(self, msg, flags);
}

int xs_sock_recvmsg (xs_sock *self, xs_msg *msg, int flags)
{
    return self->pattern->recv(self, msg, flags);
}

int xs_socket_add_stream(void *sock, void *stream) {
    //  TODO(tailhook) register all streams for socket
    return ((xs_sock *)sock)->pattern->add_stream(sock, stream);
}

