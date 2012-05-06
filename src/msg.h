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

#ifndef XS_MSG_INCLUDED
#define XS_MSG_INCLUDED

#include <stddef.h>

#define XS_MAX_VSM_SIZE 30

#define XS_MSGTYPE_VSM 101
#define XS_MSGTYPE_EXCLUSIVE 102
#define XS_MSGTYPE_SHARED 103

typedef struct
{
    unsigned char *data;
    size_t size;
    int refcnt;
} xs_content;

typedef union {
    struct {
        unsigned char unused [XS_MAX_VSM_SIZE + 1];
        unsigned char type;
    } base;
    struct {
        unsigned char data [XS_MAX_VSM_SIZE];
        unsigned char size;
        unsigned char type;
    } vsm;
    struct {
        xs_content *content;
        unsigned char unused [XS_MAX_VSM_SIZE + 1 - sizeof (xs_content*)];
        unsigned char type;
    } lmsg;
} xs_msg;

int xs_msg_init (xs_msg *self, size_t size);
void xs_msg_term (xs_msg *self);

unsigned char *xs_msg_data (xs_msg *self);
size_t xs_msg_size (xs_msg *self);

#endif
