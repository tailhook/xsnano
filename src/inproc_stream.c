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

#include "inproc_stream.h"
#include "plugin.h"


int inproc_send (void *stream, xs_msg *msg, int flags) {
    xs_inproc_stream *self = xs_stream_get_data (stream);
    if (!self->outpipe) {
        return -EPIPE;
    }
    int len = xs_msg_size(msg);
    int rc = xs_msg_pipe_quick_push (self->outpipe, *msg);
    if (rc < 0)
        return rc;
    xs_msg_init(msg, 0);
    // if (!(flags & XS_SNDMORE))
    //     return 0;
    rc = xs_msg_pipe_flush (self->outpipe);
    if (rc < 0)
        return rc;
    if (rc) {
        //  TODO(tailhook) wake up pipe using eventfd
    }
    return len;
}

int inproc_recv (void *stream, xs_msg *msg, int flags) {
    xs_inproc_stream *self = xs_stream_get_data (stream);
    xs_msg_term(msg);
    int rc = xs_msg_pipe_front(&self->inpipe, msg);
    if(rc < 0)
        return rc;
    return xs_msg_size(msg);
}


xs_stream_plugin xs_inproc_stream_plugin = {
    /* type */ XS_PLUGIN_STREAM,
    /* version */ 1,
    /* send */ inproc_send,
    /* recv */ inproc_recv
};
