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

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>

#include "xs.h"
#include "err.h"

static size_t message_size = 100;
static int message_count = 100000;

static void *worker (void *null) {
    int rc;
    char buf[message_size];

    int s = xs_socket (XS_PAIR);
    errno_assert (s);

    rc = xs_connect (s, "inproc://latency-test");
    errno_assert (rc >= 0);

    int i;
    for (i = 0; i != message_count; i++) {
        rc = xs_send (s, buf, message_size, 0);
        errno_assert (rc == message_size);
    }

    xs_close (s);

    return NULL;
}

int main (int argc, char **argv)
{
    if (argc > 3) {
        printf ("usage: inproc_throughput <message-size> <message-count>\n");
        return 1;
    }
    if (argc > 1) {
        message_size = atoi (argv [1]);
        if (argc > 2) {
            message_count = atoi (argv [2]);
        }
    }

    char buf[message_size];

    int rc;
    pthread_t worker_thread;

    rc = xs_init ();
    errno_assert (rc == 0);

    int s = xs_socket (XS_PAIR);
    errno_assert (s >= 0);

    rc = xs_bind (s, "inproc://latency-test");
    errno_assert (rc >= 0);

    rc = pthread_create (&worker_thread, NULL, worker, NULL);
    errno_assert (rc == 0);

    memset (buf, 0, message_size);

    printf ("message size: %d [B]\n", (int) message_size);
    printf ("message count: %d\n", (int) message_count);

    struct timespec tv;
    rc = clock_gettime (CLOCK_MONOTONIC, &tv);
    errno_assert (rc == 0);
    uint64_t start = (tv.tv_sec * (uint64_t) 1000000 + tv.tv_nsec / 1000);

    int i;
    for (i = 0; i != message_count; i++) {
        rc = xs_recv (s, buf, message_size, 0);
        errno_assert (rc == message_size);
    }

    rc = clock_gettime (CLOCK_MONOTONIC, &tv);
    errno_assert (rc == 0);
    uint64_t finish = (tv.tv_sec * (uint64_t) 1000000 + tv.tv_nsec / 1000);

    uint64_t elapsed = finish - start;
    if (elapsed == 0)
        elapsed = 1;

    rc = pthread_join (worker_thread, NULL);
    errno_assert (rc >= 0);

    unsigned long throughput = (unsigned long)
        ((double) message_count / (double) elapsed * 1000000);
    double megabits = (double) (throughput * message_size * 8) / 1000000;

    printf ("mean throughput: %d [msg/s]\n", (int) throughput);
    printf ("mean throughput: %.3f [Mb/s]\n", (double) megabits);

    rc = xs_close (s);
    errno_assert (rc == 0);

    rc = xs_term ();
    errno_assert (rc == 0);

    return 0;
}
