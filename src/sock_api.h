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

/*
    This module includes functions and declarations which will become a part
    of plugin API, they must *not* be used in application code

*/
#ifndef XS_SOCK_API_INCLUDED
#define XS_SOCK_API_INCLUDED


//  Bits for the state variable
#define XS_STATE_READABLE 1
#define XS_STATE_WRITEABLE 2

//  States for the update_state() method
#define XS_BECOME_READABLE 1
#define XS_BECOME_WRITEABLE 2


int xs_sock_add_stream(void *socket, void *stream);
void xs_sock_update_state (void *sock, int state);
void xs_sock_lock(void *sock);
void xs_sock_unlock(void *sock);

#endif // XS_SOCK_API_INCLUDED

