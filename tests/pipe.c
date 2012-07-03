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

#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <poll.h>

#include "../include/xs.h"
#include "../src/signal.h"
#include "../src/err.h"

#define XS_PIPE_TYPE int
#define XS_PIPE_GRANULARITY 16
#define XS_PIPE_NAME xs_intpipe
#include "pipe_template.h"

xs_intpipe main2child;
xs_signal event;


void *thread_func (void *ptr) {
    // initially queue is empty
    int value;
    int sum = 0;
    int rc;
    while(1) {
        xs_signal_wait (&event);
        value = 0;
        do {
            rc = xs_intpipe_pop (&main2child, &value);
            if (rc == 0) {
                if (value == 999111999)  // shutdown code
                    return (void *)(size_t) sum;
                sum += value;
            }
        } while (rc == 0);
        assert (rc == -EAGAIN);
    }
    return NULL;
}

int main ()
{

    pthread_t worker;
    int rc;

    rc = xs_signal_init (&event);
    err_assert (rc);
    rc = xs_intpipe_init (&main2child);
    err_assert (rc);

    pthread_create (&worker, NULL, thread_func, NULL);

    // First message should produce wakeup
    rc = xs_intpipe_push (&main2child, 1);
    xs_assert (rc == 1);

    // We have not woke up yet, but expect the wake up is already queued
    rc = xs_intpipe_push (&main2child, 2);
    err_assert (rc);

    // Let's try quickpush
    rc = xs_intpipe_quick_push (&main2child, 3);
    err_assert (rc);

    // Let's do real wakeup
    rc = xs_signal_wakeup(&event);
    err_assert (rc);

    // Sleeping a little
    rc = poll(NULL, 0, 100);
    xs_assert (rc == 0);

    // Let's check that we need to wake up again
    rc = xs_intpipe_push (&main2child, 7);
    xs_assert (rc == 1);

    // Lets send the shutdown signal
    rc = xs_intpipe_push (&main2child, 999111999);
    err_assert (rc);

    rc = xs_signal_wakeup (&event);
    err_assert (rc);

    size_t result;
    rc = pthread_join (worker, (void **)&result);
    assert (rc == 0);
    assert (result == 13);

    xs_intpipe_free (&main2child);

    //////////////////////////////////////
    // Now lets send a lot of messsages
    //
    xs_intpipe_init (&main2child);
    pthread_create (&worker, NULL, thread_func, NULL);

    int i;
    for(i = 0; i < 10000; ++i) {
        rc = xs_intpipe_push (&main2child, i);
        if (rc) {
            rc = xs_signal_wakeup (&event);
            err_assert (rc);
        }
    }

    // Lets send the shutdown signal
    rc = xs_intpipe_push (&main2child, 999111999);
    if (rc) {
        rc = xs_signal_wakeup (&event);
        err_assert (rc);
    }

    rc = pthread_join (worker, (void **)&result);
    assert (rc == 0);
    assert (result == 49995000);

    rc = xs_intpipe_free (&main2child);
    assert (rc == 0);

    xs_signal_term (&event);

    return 0;
}
