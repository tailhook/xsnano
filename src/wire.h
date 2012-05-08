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

#ifndef XS_WIRE_INCLUDED
#define XS_WIRE_INCLUDED

#include <stdint.h>

inline uint64_t xs_getll (unsigned char *buf)
{
    return (((uint64_t) buf [0]) << 56) |
           (((uint64_t) buf [1]) << 48) |
           (((uint64_t) buf [2]) << 40) |
           (((uint64_t) buf [3]) << 32) |
           (((uint64_t) buf [4]) << 24) |
           (((uint64_t) buf [5]) << 16) |
           (((uint64_t) buf [6]) << 8) |
           (((uint64_t) buf [7] << 0));
}

inline void xs_putll (unsigned char *buf, uint64_t val)
{
    buf [0] = (unsigned char) ((val >> 56) & 0xff);
    buf [1] = (unsigned char) ((val >> 48) & 0xff);
    buf [2] = (unsigned char) ((val >> 40) & 0xff);
    buf [3] = (unsigned char) ((val >> 32) & 0xff);
    buf [4] = (unsigned char) ((val >> 24) & 0xff);
    buf [5] = (unsigned char) ((val >> 16) & 0xff);
    buf [6] = (unsigned char) ((val >> 8) & 0xff);
    buf [7] = (unsigned char) (val & 0xff);
}

#endif

