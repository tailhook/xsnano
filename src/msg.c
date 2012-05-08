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

#include <stdlib.h>

#include "msg.h"
#include "err.h"

int xs_msg_init (xs_msg *self, size_t size)
{
    if (size <= XS_MAX_VSM_SIZE) {
        self->vsm.type = XS_MSGTYPE_VSM;
        self->vsm.size = (unsigned char) size;
        return 0;
    }

    self->lmsg.type = XS_MSGTYPE_EXCLUSIVE;
    self->lmsg.content = malloc (sizeof (xs_content) + size);
    if (!self->lmsg.content)
        return -ENOMEM;

    self->lmsg.content->data = self->lmsg.content + 1;
    self->lmsg.content->size = size;
    self->lmsg.content->refcnt = 1;

    return 0;
}

void xs_msg_term (xs_msg *self)
{
    if (self->base.type == XS_MSGTYPE_VSM) {
        self->base.type = 0;
        return;
    }

    if (self->base.type == XS_MSGTYPE_SHARED) {

        /*  TODO: This should be an atomic operation! */
        self->lmsg.content->refcnt--;
        if (self->lmsg.content->refcnt)
            return;
    }

    free (self->lmsg.content);
    self->base.type = 0;
}

unsigned char *xs_msg_data (xs_msg *self)
{
    if (self->base.type == XS_MSGTYPE_VSM)
        return self->vsm.data;
    return self->lmsg.content->data;
}

size_t xs_msg_size (xs_msg *self)
{
    if (self->base.type == XS_MSGTYPE_VSM)
        return self->vsm.size;
    return self->lmsg.content->size;
}

void xs_msg_move (xs_msg *self, xs_msg *src)
{
    int rc;

    xs_msg_term (self);
    *self = *src;
    rc = xs_msg_init (src, 0);
    errno_assert (rc == 0);
}

