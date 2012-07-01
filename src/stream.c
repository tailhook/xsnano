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

#include "stream.h"
#include "err.h"

int xs_stream_send (void *stream, xs_msg *msg, int flags) {
    return ((xs_stream *)stream)->plugin->send(stream, msg, flags);
}

int xs_stream_recv (void *stream, xs_msg *msg, int flags) {
    return ((xs_stream *)stream)->plugin->recv(stream, msg, flags);
}

int xs_stream_create (void **stream) {
    xs_stream *self = malloc (sizeof (xs_stream));
    if(!self)
        return -ENOMEM;
    self->socket = NULL;
    self->plugin = NULL;
    *stream = self;
    return 0;
}

void xs_stream_term (void *stream) {
    xs_stream *self = stream;
    xs_assert (!self->socket); // not implemented yet
    xs_assert (!self->plugin); // not implemented yet
    free(stream);
}

void xs_stream_set_plugin (void *stream, void *plugin) {
    xs_stream *self = stream;
    xs_assert (!self->plugin);
    self->plugin = plugin;
}

void xs_stream_set_data (void *stream, void *data) {
    xs_stream *self = stream;
    self->userdata = data;
}

void *xs_stream_get_data (void *stream) {
    xs_stream *self = stream;
    return self->userdata;
}

