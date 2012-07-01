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

#include "pair_pattern.h"
#include "pattern_plugin.h"
#include "transport_func.h"
#include "err.h"
#include "stream_plugin.h"
#include "stream.h"

typedef struct {
    void *stream;
} xs_pair_data;

int init_socket (void *socket) {
    xs_pair_data *str = malloc (sizeof (xs_pair_data));
    if (!str)
        return -ENOMEM;
    str->stream = NULL;
    xs_pattern_set_data(socket, str);
    return 0;
}

int close_socket (void *socket) {
    xs_pair_data *str = (xs_pair_data*) xs_pattern_get_data (socket);
    //  TODO(tailhook) do we need to shutdown a stream manually?
    free(str);
    return 0;
}

int check_transport (void *socket, void *transport) {
    return 0;  // all allowed temporarily
}

int add_stream (void *socket, void *stream) {
    xs_pair_data *str = (xs_pair_data*) xs_pattern_get_data (socket);
    if (str->stream)
        return -EBUSY;
    str->stream = stream;
    return 0;
}

int pair_send (void *socket, xs_msg *msg, int flags) {
    xs_pair_data *self = (xs_pair_data*) xs_pattern_get_data (socket);
    if (!self->stream)
        return -EAGAIN;
    return xs_stream_send(self->stream, msg, flags);
}

int pair_recv (void *socket, xs_msg *msg, int flags) {
    xs_pair_data *self = (xs_pair_data*) xs_pattern_get_data (socket);
    if (!self->stream)
        return -EAGAIN;
    return xs_stream_recv(self->stream, msg, flags);
}

static xs_pattern_plugin xs_pair_pattern_struct = {
    /* type */ XS_PLUGIN_PATTERN,
    /* version */ 1,
    /* socket_type */ XS_PAIR,
    /* pattern */ XS_PAIR,  // not sure about this one
    /* role */ 1,  // not sure about this one
    /* init_socket */ init_socket,
    /* close_socket */ close_socket,
    /* set_option */ NULL,
    /* get_option */ NULL,
    /* check_transport */ check_transport,
    /* add_stream */ add_stream,
    /* send */ pair_send,
    /* recv */ pair_recv
};

xs_base_plugin *xs_pair_pattern () {
    return (xs_base_plugin*)&xs_pair_pattern_struct;
}
