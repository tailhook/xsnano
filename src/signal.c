/*
    Copyright (c) 2012 Paul Colomiets

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

#include "signal.h"

#include <sys/eventfd.h>
#include <errno.h>
#include <unistd.h>
#include <poll.h>

#include "err.h"


int xs_signal_init (xs_signal *self) {
    self->eventfd = eventfd (0, EFD_CLOEXEC|EFD_NONBLOCK);
    if (self->eventfd < 0)
        return -errno;
    return 0;
}

void xs_signal_term (xs_signal *self) {
    close(self->eventfd);
}

int xs_signal_check (xs_signal *self) {
    int rc;
    while (1) {
        uint64_t efdval = 0;
        rc = read (self->eventfd, &efdval, sizeof (efdval));
        if (rc < 0) {
            if (errno == EAGAIN || errno == EINTR)
                continue;
            err_assert (rc);
        }
        if (rc > 0)
            return 1;
        else
            return -errno;  //  Valid also if no error
    }
}

void xs_signal_wait (xs_signal *self) {
    while (1) {
        struct pollfd pi = { self->eventfd, POLLIN, 0 };
        uint64_t efdval = 0;
        int rc = poll (&pi, 1, -1);
        if (rc < 0) {
            if (errno == EAGAIN || errno == EINTR)
                continue;
            err_assert (rc);
        }
        xs_assert (rc > 0);
        rc = read (self->eventfd, &efdval, sizeof (efdval));
        if (rc < 0) {
            if (errno == EAGAIN || errno == EINTR)
                continue;
            err_assert (rc);
        }
        break;
    }
}

int xs_signal_wakeup (xs_signal *self) {
    uint64_t one = 1;
    int rc = write (self->eventfd, &one, sizeof (one));
    if (rc < 0)
        return -errno;
    return 0;
}
