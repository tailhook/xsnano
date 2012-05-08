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
#include <string.h>

#include "tcpout.h"
#include "err.h"

static int xs_tcpout_write (xs_tcpout *self, int block)
{
    ssize_t nbytes;
    int flags = block ? 0 : MSG_DONTWAIT;
    size_t iovlen = 0;
    struct iovec iov [2];
    unsigned char sizebuf [8] = {0};

    if (self->sent < 8) {
        iov [0].iov_base = sizebuf + self->sent;
        iov [0].iov_len = 8 - self->sent;
        iov [1].iov_base = xs_msg_data (&self->msg);
        iov [1].iov_len = xs_msg_size (&self->msg);
        iovlen = 2;
    }
    else {
        iov [0].iov_base = xs_msg_data (&self->msg) + (self->sent - 8);
        iov [0].iov_len = xs_msg_size (&self->msg) - (self->sent - 8);
        iovlen = 1;        
    }

    struct msghdr hdr = {NULL, 0, iov, iovlen, NULL, 0, 0};
    nbytes = sendmsg (self->fd, &hdr, flags | MSG_NOSIGNAL);

    /*  Sanitation of the result. */
    if (unlikely (nbytes == 0)) {
        errno = ECONNREFUSED;
        nbytes = -1;
    }
    else if (unlikely (nbytes == -1 &&
          (errno == EAGAIN || errno == EWOULDBLOCK)))
        nbytes = 0;

    errno_assert (nbytes >= 0);

    self->sent += nbytes;

    /*  If message is fully sent. */
    if (self->sent == xs_msg_size (&self->msg) + 8) {
        self->sent = 0;
        return 0;
    }

    /*  There are more data to send. */
    return -EAGAIN;
}

static void *xs_tcpout_worker (void *arg)
{
    int rc;
    xs_tcpout *self = (xs_tcpout*) arg;

    /*  Send the data. */
    rc = xs_tcpout_write (self, 1);
    err_assert (rc);

    self->busy = 0;

    /*  Notify the socket core that the data have been sent. */
    self->done (self->arg);

    return NULL;
}

int xs_tcpout_init (xs_tcpout *self, int fd, void (*done) (void*), void *arg)
{
    self->fd = fd;
    self->busy = 0;
    self->done = done;
    self->arg = arg;
}

void xs_tcpout_term (xs_tcpout *self)
{
    /*  TODO: Cancel async operations here. */
}

int xs_tcpout_send (xs_tcpout *self, xs_msg *msg)
{
    int rc;

    /*  Asynchronous send already in progress. */
    if (self->busy)
        return -EAGAIN;

    /*  Move the message ownership from the user to the XS. */
    xs_msg_move (&self->msg, msg);
    self->sent = 0;

    /*  Try to send the message in a synchronous manner. */
    rc = xs_tcpout_write (self, 0);

    /*  Start the asynchronous operation. */
    if (rc == -EAGAIN) {
        self->busy = 1;
        rc = pthread_create (&self->worker, NULL, xs_tcpout_worker, self);
        errnum_assert (rc);
        return -EINPROGRESS;
    }

    /*  Everything is OK. Message sent. */
    err_assert (rc);
    return 0;
}

