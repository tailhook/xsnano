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

#ifndef XS_STREAM_INCLUDED
#define XS_STREAM_INCLUDED

#include "sock.h"
#include "stream_plugin.h"


typedef struct xs_stream {
    xs_sock *socket;
    xs_stream_plugin *plugin;
    void *userdata;
} xs_stream;


int xs_stream_send (void *stream, xs_msg *msg, int flags);
int xs_stream_recv (void *stream, xs_msg *msg, int flags);
int xs_stream_create (void **stream);
void xs_stream_term (void *stream);
void xs_stream_set_plugin (void *stream, void *plugin);
void xs_stream_set_data (void *stream, void *data);
void *xs_stream_get_data (void *stream);

#endif // XS_STREAM_INCLUDED
