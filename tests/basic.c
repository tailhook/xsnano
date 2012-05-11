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

#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#include "../include/xs.h"

int main ()
{
    int rc;
    int s;

    printf ("basic test running...\n");

    rc = xs_init ();
    assert (rc == 0);

    s = xs_socket (XS_XPUB);
    assert (s >= 0);

    rc = xs_send (s, "ABC", 3, 0);
    assert (rc == 3);

    sleep (1);

    char buf [32];
    rc = xs_recv (s, buf, sizeof (buf), 0);
    assert (rc == 3);

    rc = xs_close (s);
    assert (rc == 0);

    rc = xs_term ();
    assert (rc == 0);

    return 0;
}
