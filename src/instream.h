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

#ifndef XS_INSTREAM_INCLUDED
#define XS_INSTREAM_INCLUDED

#include <pthread.h>

#include "msg.h"

typedef struct {

    /*  The underlying socket. */
    int fd;

    /*  State of the processing. 0 means there's no message available and no
        background processing going on. 1 means there is no message to be had
        but the background read is going on. 2 means there's message available
        ie. 'msg' member is filled in. */
    int state;

    /*  Message being curretly read and number of bytes already read. */
    xs_msg msg;
    int recvd;

    /*  Buffer to read size of the message to. */
    unsigned char sizebuf [8];

    /*  Thread handling the asynchronous operations. */
    pthread_t worker;

    /*  Callback to be invoked when asynchronous send succeeds. */
    void (*done) (void*);
    void *arg;

} xs_instream;

int xs_instream_init (xs_instream *self, int fd, void (*done) (void*),
    void *arg);
void xs_instream_term (xs_instream *self);

int xs_instream_recv (xs_instream *self, xs_msg *msg);

#endif

