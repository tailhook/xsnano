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

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>

#include "instream.h"
#include "wire.h"
#include "err.h"

static int xs_instream_read (xs_instream *self)
{
}

int xs_instream_init (xs_instream *self, int fd, void (*done) (void*),
    void *arg)
{
    self->fd = fd;
    self->state = 0;
    self->done = done;
    self->arg = arg;
}

void xs_instream_term (xs_instream *self)
{
    /*  TODO: Stop any async operations here. */
}

int xs_instream_recv (xs_instream *self, xs_msg *msg)
{
    int rc;
    ssize_t nbytes;

    /*  If there's a message available, return is straight away. */
    if (self->state == 2) {
        xs_msg_move (msg, &self->msg);
        self->state = 0;
        return 0;
    }

    /*  If message is being asynchronously received at the moment, there's
        no message to return yet. */
    if (self->state == 1)
        return -EINPROGRESS;

    /* Try to read the message in a synchronous way. */
    nbytes = recv (self->fd, self->sizebuf, 8, MSG_DONTWAIT);

    /*  Sanitation of the result. */
    if (unlikely (nbytes == 0)) {
        errno = ECONNREFUSED;
        nbytes = -1;
    }
    else if (unlikely (nbytes == -1 &&
          (errno == EAGAIN || errno == EWOULDBLOCK)))
        nbytes = 0;

    errno_assert (nbytes >= 0);

    if (nbytes < 8) {
        self->recvd = nbytes;
        /*  TODO: xs_instream_launch_async (self); */
        return -EINPROGRESS;
    }

    rc = xs_msg_init (&self->msg, xs_getll (self->sizebuf));
    err_assert (rc);

    nbytes = recv (self->fd, xs_msg_data (&self->msg), xs_msg_size (&self->msg),
        MSG_DONTWAIT);

    /*  Sanitation of the result. */
    if (unlikely (nbytes == 0)) {
        errno = ECONNREFUSED;
        nbytes = -1;
    }
    else if (unlikely (nbytes == -1 &&
          (errno == EAGAIN || errno == EWOULDBLOCK)))
        nbytes = 0;

    if (nbytes < xs_msg_size (&self->msg)) {
        self->recvd = nbytes + 8;
        /*  TODO: xs_instream_launch_async (self); */
        return -EINPROGRESS;
    }

    /*  Synchronous receive was successful. */
    xs_msg_move (msg, &self->msg);
    return 0;
}

