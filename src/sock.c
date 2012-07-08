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
#include "sock_api.h"
#include "err.h"
#include "likely.h"
#include "msg.h"
#include "stream.h"
#include "transport_func.h"
#include "threadlocal.h"


int xs_sock_init (xs_sock *self)
{
    self->state = 0;
    xs_mutex_init (&self->sync);
    LIST_INIT (&self->listeners);
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
    xs_sock_lock(self);
    int rc = self->pattern->send(self, msg, flags);
    if(rc == -EAGAIN && !(flags & XS_DONTWAIT)) {
        xs_threadlocal_data *data;
        rc = xs_threadlocal_get(&self->ctx->threadlocal, &data);
        if(rc < 0) {
            xs_sock_unlock(self);
            return rc;
        }
        xs_listener listener;
        listener.signal = &data->signal;
        listener.mask = XS_STATE_WRITEABLE;
        LIST_INSERT_HEAD (&self->listeners, &listener, list);
        do {
            self->state &= ~XS_STATE_WRITEABLE;
            xs_sock_unlock (self);
            xs_signal_wait (&data->signal);
            xs_sock_lock (self);
            rc = self->pattern->send(self, msg, flags);
        } while (rc == -EAGAIN);
        LIST_REMOVE (&listener, list);
    }
    xs_sock_unlock (self);
    return rc;
}

int xs_sock_recvmsg (xs_sock *self, xs_msg *msg, int flags)
{
    xs_sock_lock(self);
    int rc = self->pattern->recv(self, msg, flags);
    if(rc == -EAGAIN && !(flags & XS_DONTWAIT)) {
        xs_threadlocal_data *data;
        rc = xs_threadlocal_get(&self->ctx->threadlocal, &data);
        if(rc < 0) {
            xs_sock_unlock(self);
            return rc;
        }
        xs_listener listener;
        listener.signal = &data->signal;
        listener.mask = XS_STATE_READABLE;
        LIST_INSERT_HEAD (&self->listeners, &listener, list);
        do {
            self->state &= ~XS_STATE_READABLE;
            xs_sock_unlock (self);
            xs_signal_wait (&data->signal);
            xs_sock_lock (self);
            rc = self->pattern->recv(self, msg, flags);
        } while (rc == -EAGAIN);
        LIST_REMOVE (&listener, list);
    }
    xs_sock_unlock(self);
    return rc;
}

int xs_sock_add_stream (void *sock, void *stream) {
    //  TODO(tailhook) register all streams for socket
    ((xs_stream *)stream)->socket = sock;
    return ((xs_sock *)sock)->pattern->add_stream (sock, stream);
}

void xs_sock_lock (void *sock) {
    xs_mutex_lock (&((xs_sock *)sock)->sync);
}

void xs_sock_unlock(void *sock) {
    xs_mutex_unlock (&((xs_sock *)sock)->sync);
}

static void sock_wakeup (xs_sock *sock, int bit) {
    xs_listener *item;
    LIST_FOREACH (item, &sock->listeners, list) {
        if (item->mask & bit) {
            xs_signal_wakeup(item->signal);
        }
    }
}

// must be called under mutex
void xs_sock_update_state (void *sock, int state) {
    xs_sock *self = sock;
    switch (state) {
    case XS_BECOME_READABLE:
        if (self->state & XS_STATE_READABLE) // already readable
            return;
        self->state |= XS_STATE_READABLE;
        sock_wakeup(self, XS_STATE_READABLE);
        break;
    case XS_BECOME_WRITEABLE:
        if (self->state & XS_STATE_WRITEABLE) // already writeable
            return;
        self->state |= XS_STATE_WRITEABLE;
        sock_wakeup(self, XS_STATE_WRITEABLE);
        break;
    }
}


