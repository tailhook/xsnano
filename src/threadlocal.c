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

#include <malloc.h>
#include <errno.h>

#include "threadlocal.h"


void threadlocal_free(void *data) {
    if (!data)
        return;
    xs_threadlocal_data *val = data;
    xs_mutex_lock (&val->parent->sync);
    LIST_REMOVE (val, list);
    xs_mutex_unlock (&val->parent->sync);
    xs_signal_term (&val->signal);
    free (val);
}

int xs_threadlocal_init (xs_threadlocal *tl) {
    xs_mutex_init (&tl->sync);
    return pthread_key_create (&tl->key, threadlocal_free);
}

int xs_threadlocal_term (xs_threadlocal *tl) {
    pthread_key_delete (tl->key);
    xs_threadlocal_data *item, *next;
    for(item = LIST_FIRST(&tl->registry); item; item=next) {
        next = LIST_NEXT(item, list);
        xs_signal_term (&item->signal);
        free (item);
    }
    return 0;
}

int xs_threadlocal_get (xs_threadlocal *tl, xs_threadlocal_data **data) {
    xs_threadlocal_data *val;
    val = (xs_threadlocal_data *)pthread_getspecific (tl->key);
    if (!val) {
        val = malloc (sizeof (xs_threadlocal_data));
        if (!val)
            return -ENOMEM;
        int rc = xs_signal_init (&val->signal);
        if (rc < 0) {
            free(val);
            return rc;
        }
        val->parent = tl;
        xs_mutex_lock (&tl->sync);
        LIST_INSERT_HEAD (&tl->registry, val, list);
        xs_mutex_unlock (&tl->sync);
        pthread_setspecific(tl->key, val);
    }
    *data = val;
    return 0;
}
