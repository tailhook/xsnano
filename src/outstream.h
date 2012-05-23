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

#ifndef XS_OUTSTREAM_INCLUDED
#define XS_OUTSTREAM_INCLUDED

#include <pthread.h>

#include "msg.h"

#if defined SO_NOSIGPIPE
    /*   Define a phony MSG_NOSIGNAL for OSX, which does not include it */
    #define MSG_NOSIGNAL 0
#endif

typedef struct {

    /*  The underlying socket. */
    int fd;

    /*  If 1 there is asynchronous write going on in the background. */
    int busy;

    /*  Message being sent and number of bytes already sent. */
    xs_msg msg;
    int sent;

    /*  Thread handling the asynchronous operations. */    
    pthread_t worker;

    /*  Callback to be invoked when asynchronous send succeeds. */
    void (*done) (void*);
    void *arg;

} xs_outstream;

int xs_outstream_init (xs_outstream *self, int fd, void (*done) (void*),
    void *arg);
void xs_outstream_term (xs_outstream *self);

int xs_outstream_send (xs_outstream *self, xs_msg *msg);

#endif
