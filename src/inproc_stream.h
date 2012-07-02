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

#ifndef XS_INPROC_STREAM_INCLUDED
#define XS_INPROC_STREAM_INCLUDED

#include "msg.h"
#include "stream_plugin.h"

#define XS_PIPE_TYPE xs_msg
#define XS_PIPE_GRANULARITY 256
#define XS_PIPE_NAME xs_msg_pipe
#include "pipe_template.h"


typedef struct xs_inproc_stream {
    xs_msg_pipe inpipe;
    xs_msg_pipe *outpipe;
    struct xs_inproc_stream *otherstream;
} xs_inproc_stream;


extern xs_stream_plugin xs_inproc_stream_plugin;


#endif // XS_INPROC_STREAM_INCLUDED
