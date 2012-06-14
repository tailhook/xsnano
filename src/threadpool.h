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

#ifndef XS_THREADPOOL_INCLUDED
#define XS_THREADPOOL_INCLUDED

#include <pthread.h>

#include "commands.h"

enum xs_thread_status {
    XS_THREAD_STARTING,
    XS_THREAD_WORKING,
    XS_THREAD_STOPPING,
    XS_THREAD_STOPPED
};

typedef struct {
    pthread_t posix_id;
    enum xs_thread_status status;
    int load;
    int signalfd;
    xs_cmdpipe pipe;
} xs_thread;

typedef struct {
    int num_threads;
    xs_thread **threads;
} xs_threadpool;

int xs_threadpool_init(xs_threadpool *pool);
int xs_threadpool_ensure_ready(xs_threadpool *pool);
int xs_threadpool_resize(xs_threadpool *pool, int size);
int xs_threadpool_shutdown(xs_threadpool *pool);

#endif // XS_THREADPOOL_INCLUDED
