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

/* This is a template pipe. Example usage is as follows:

    #define XS_PIPE_TYPE int
    #define XS_PIPE_GRANULARITY 16
    #define XS_PIPE_NAME xs_int_pipe
    #include "pipe_template.h"


    The pipe is unidirectional lock-free single-producer single-consumer queue
    You need to have an external way to wake up consumer, after it fall asleep
    because there are no more messages.

    Parameters:
        XS_PIPE_TYPE -- is the type of object stored in queue
        XS_PIPE_GRANULARITY -- is number of elements in each chunk of queue
        XS_PIPE_NAME -- the name of the pipe struct. *And* a prefix for methods

    Defined methods:
        PIPENAME_init -- initialized queue
        PIPENAME_free -- frees chunk memory, data must be freed before
        PIPENAME_push -- writes a message to a queue, returns true if reader
                         should be woken up
        PIPENAME_quick_push -- pushes message without propagating to reader
                               (useful for bulk pushes)
        PIPENAME_flush -- propagates quick-pushed message to reader. Returns
                          true if reader should be woken up
        PIPENAME_pop -- pops an element from a queue, optionally returns value
        PIPENAME_front -- inspects element before popping

*/

#include <malloc.h>
#include <stdint.h>
#include <errno.h>

#include "atomic.h"

// checking user-specified macros
#ifndef XS_PIPE_TYPE
#error XS_PIPE_TYPE is not defined
#endif
#ifndef XS_PIPE_GRANULARITY
#error XS_PIPE_GRANULARITY is not defined
#endif
#ifndef XS_PIPE_NAME
#error XS_PIPE_GRANULARITY is not defined
#endif

// local macros
#define XS__NAME2(a, b) a ## _ ## b
#define XS__NAME1(a, b) XS__NAME2 (a, b)
#define XS__NAME(name) XS__NAME1 (XS_PIPE_NAME, name)

typedef struct XS__NAME (chunk) {
    struct XS__NAME(chunk) *next;
    XS_PIPE_TYPE data [XS_PIPE_GRANULARITY];
} XS__NAME(chunk);

typedef struct {
    XS__NAME(chunk) *first_chunk;
    int begin_pos;

    // Counter of the messages that can be safefully read
    unsigned to_read;
    // Last value of a buffer to be subtracted from buffer
    unsigned last_buffer;

    // The only place of contention between reader and writer
    // this counter is exchanged with to_read by reader and with to_write
    // by writer, when data is flushed
    volatile unsigned buffer __attribute__((aligned(64)));

    // Number of messages in the queue, that aren't ready to flush
    unsigned to_write __attribute__((aligned(64)));
    int end_pos;
    XS__NAME(chunk) **last_chunk;
} XS_PIPE_NAME;


static inline int XS__NAME (init) (XS_PIPE_NAME *pipe) {
    XS__NAME (chunk) *chunk = malloc (sizeof (XS__NAME (chunk)));
    if(!chunk)
        return -ENOMEM;
    chunk->next = NULL;
    pipe->first_chunk = chunk;
    pipe->last_chunk = &chunk->next;
    pipe->begin_pos = 0;
    pipe->end_pos = 0;
    pipe->to_read = 0;
    pipe->last_buffer = 0;
    pipe->buffer = 0;
    pipe->to_write = 0;
    return 0;
}

static inline int XS__NAME (free) (XS_PIPE_NAME *pipe) {
    XS__NAME (chunk) *chunk, *next;
    for (chunk = pipe->first_chunk; chunk; chunk=next) {
        next = chunk->next;
        free(chunk);
    }
    return 0;
}

static inline int XS__NAME (quick_push) (XS_PIPE_NAME *pipe, XS_PIPE_TYPE value) {
    XS__NAME (chunk) *chunk;
    if (pipe->end_pos >= XS_PIPE_GRANULARITY) {
        chunk = malloc (sizeof (XS__NAME (chunk)));
        if(!chunk)
            return -ENOMEM;
        chunk->next = NULL;
        *pipe->last_chunk = chunk;
        pipe->last_chunk = &chunk->next;
        pipe->end_pos = 0;
    } else {
        chunk = (XS__NAME(chunk) *) (
            (char *)pipe->last_chunk - offsetof (XS__NAME(chunk), next));
    }
    chunk->data [pipe->end_pos++] = value;
    pipe->to_write ++;
    return 0;
}

static inline int XS__NAME (flush) (XS_PIPE_NAME *pipe) {
    if (pipe->to_write) {
        unsigned oldval = XS_ATOMIC_FETCH_ADD (pipe->buffer, pipe->to_write);
        pipe->to_write = 0;
        if(oldval > 0)
            return 0;
        return 1; // Wake up reader
    }
    return 0;
}

static inline int XS__NAME (push) (XS_PIPE_NAME *pipe, XS_PIPE_TYPE value) {
    int rc = XS__NAME (quick_push) (pipe, value);
    if (rc) {
        return rc;
        }
    rc = XS__NAME (flush) (pipe);
    return rc;
}

static inline int XS__NAME (_check_read) (XS_PIPE_NAME *pipe) {
    if(!pipe->to_read) {
        pipe->to_read = pipe->last_buffer = XS_ATOMIC_SUB_FETCH(
            pipe->buffer, pipe->last_buffer);
        if(!pipe->to_read)
            return -EAGAIN;
    }
    if(pipe->begin_pos >= XS_PIPE_GRANULARITY) {
        XS__NAME (chunk) *chunk = pipe->first_chunk;
        if ((pipe->first_chunk = chunk->next) == NULL)
            pipe->last_chunk = &pipe->first_chunk;
        free(chunk);
        pipe->begin_pos = 0;
    }
    return 0;
}

static inline int XS__NAME (pop) (XS_PIPE_NAME *pipe, XS_PIPE_TYPE *value) {
    int rc = XS__NAME (_check_read) (pipe);
    if(rc)
        return rc;
    if(value)
        *value = pipe->first_chunk->data [pipe->begin_pos];
    pipe->begin_pos ++;
    pipe->to_read --;
    return 0;
}

static inline int XS__NAME (front) (XS_PIPE_NAME *pipe, XS_PIPE_TYPE *value) {
    int rc = XS__NAME (_check_read) (pipe);
    if(rc)
        return rc;
    *value = pipe->first_chunk->data [pipe->begin_pos];
    return 0;
}


// undefining local macros
#undef XS__NAME
#undef XS__NAME1
#undef XS__NAME2

// undefining user-defined parameters
#undef XS_PIPE_TYPE
#undef XS_PIPE_GRANULARITY
#undef XS_PIPE_NAME

