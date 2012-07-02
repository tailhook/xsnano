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

#include <poll.h>
#include <errno.h>
#include <unistd.h>

#include "err.h"
#include "threadpool.h"


static void *thread_start(xs_thread *thread) {
    thread->status = XS_THREAD_WORKING;
    xs_command cmd;
    int rc;

    while (1) {
        xs_signal_wait (&thread->signal);
        while (1) {
            rc = xs_cmdpipe_pop (&thread->pipe, &cmd);
            if (rc < 0) {
                if (errno == EAGAIN || errno == EINTR)
                    break;
                err_assert (rc);
            }
            switch (cmd.cmd) {

            case XS_CMD_PING:
                // No-op command
                break;

            case XS_CMD_SHUTDOWN:
                //  Just return from the loop, all cleanup is currently done
                //  in the main loop
                return NULL;

            }
        }
    }
    return NULL;
}

int xs_threadpool_init (xs_threadpool *pool) {
    pool->num_threads = 1;
    pool->threads = NULL;
    return 0;
}

int xs_threadpool_ensure_ready (xs_threadpool *pool) {
    int i, rc;
    xs_thread *thread = NULL;
    if(pool->threads)
        return 0;
    pool->threads = malloc (sizeof (xs_thread *)*pool->num_threads);
    if(!pool->threads)
        return -ENOMEM;
    for(i = 0; i < pool->num_threads; ++i) {
        thread = malloc (sizeof (xs_thread));
        if(!thread) {
            rc = -ENOMEM;
            goto error;
        }
        thread->status = XS_THREAD_STARTING;
        thread->load = 0;

        rc = xs_cmdpipe_init (&thread->pipe);
        if(rc < 0)
            goto error_free_thread;

        rc = xs_signal_init (&thread->signal);
        if(rc < 0)
            goto error_close_pipe;

        rc = pthread_create (&thread->posix_id, NULL,
            (void *(*)(void *))thread_start, thread);
        if(rc < 0)
            goto error_close_signal;
        pool->threads[i] = thread;
    }
    return 0;
error_close_signal:
    xs_signal_term (&thread->signal);
error_close_pipe:
    xs_cmdpipe_free (&thread->pipe);
error_free_thread:
    free (thread);
error:
    if (i == 0) {
        //  When we failed to create even a single thread, we leave thread
        //  pool in uninitialized state
        free (pool->threads);
        pool->threads = NULL;
    } else {
        //  When we failed to create big number of threads, we just set
        //  the right value, so that it can be retrieved by getctxopt
        pool->num_threads = i;
    }
    return rc;
}

int xs_threadpool_resize (xs_threadpool *pool, int size) {
    if (size < 1)
        return -EINVAL;
    if (size == pool->num_threads)
        return 0;
    if (pool->threads)
        return -EALREADY;
    pool->num_threads = size;
    //  ensure_ready() returns error if not all threads are created
    //  user can then use getctxopt(...IO_THREADS...) to determine how many
    //  threads are really created
    return xs_threadpool_ensure_ready (pool);
}

int xs_threadpool_shutdown (xs_threadpool *pool) {
    if(!pool->threads)
        return 0;  // never initialized
    int rc, i;

    // Let's send shutdown command
    xs_command shutdown = { XS_CMD_SHUTDOWN };
    for (i = 0; i < pool->num_threads; ++i) {
        int status = pool->threads[i]->status;
        if(status == XS_THREAD_STARTING || status == XS_THREAD_WORKING) {
            rc = xs_cmdpipe_push (&pool->threads[i]->pipe, shutdown);
            if(rc > 0) {
                rc = xs_signal_wakeup (&pool->threads[i]->signal);
                //  There are no realistic situation where write to eventfd
                //  can fail. But watch out other signalling implementations
                err_assert (rc);
            } else {
                // No memory to send shutdown signal to pipe, what to do?
                // TODO(tailhook): probably we can wait for other threads to
                //                 shutdown and free some memory
                err_assert (rc);
            }
        }
    }

    // Now wait for thread to finish their work
    for (i = 0; i < pool->num_threads; ++i) {
        int status = pool->threads[i]->status;
        if(status != XS_THREAD_STOPPED) {
            rc = pthread_join (pool->threads[i]->posix_id, NULL);
            //  There's no way pthread_join can fail in our case, right?
            xs_assert (rc == 0);
            pool->threads[i]->status = XS_THREAD_STOPPED;
            rc = xs_cmdpipe_free (&pool->threads[i]->pipe);
            xs_assert (rc == 0);
            xs_signal_term (&pool->threads[i]->signal);
            free (pool->threads[i]);
            pool->threads[i] = NULL;
        }
    }

    // Put threadpool in unitialized state
    free(pool->threads);
    pool->threads = NULL;
    return 0;
}
