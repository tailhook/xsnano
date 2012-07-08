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

#ifndef XS_THREADLOCAL_INCLUDED
#define XS_THREADLOCAL_INCLUDED

#include <pthread.h>
#include <sys/queue.h>

#include "mutex.h"
#include "signal.h"


typedef struct {
    pthread_key_t key;

    //  When destroing pthread_key_t there is no way to enumerate all the
    //  thread local instances, so we keep them in a list
    LIST_HEAD (xs_threadlocal_head, xs_threadlocal_data) registry;

    // List syncronization
    xs_mutex sync;

} xs_threadlocal;

typedef struct xs_threadlocal_data {
    xs_threadlocal *parent;
    xs_signal signal;
    LIST_ENTRY (xs_threadlocal_data) list;
} xs_threadlocal_data;

int xs_threadlocal_init (xs_threadlocal *);
int xs_threadlocal_term (xs_threadlocal *);
int xs_threadlocal_get (xs_threadlocal *, xs_threadlocal_data **data);

#endif // XS_THREADLOCAL_INCLUDED
