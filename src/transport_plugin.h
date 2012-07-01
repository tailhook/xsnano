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

#ifndef XS_TRANSPORT_PLUGIN_INCLUDED
#define XS_TRANSPORT_PLUGIN_INCLUDED

#include <stdlib.h>
#include <sys/queue.h>

#include "msg.h"


typedef struct xs_transport_plugin {
    //  base plugin
    int type;        //  = XS_PLUGIN_TRANSPORT
    int version;
    //  end of base plugin

    // The name of the transport
    char *name;

    // Returns the instance of a bound transport
    int (*bind)(void *transport, const char *addr);
    // Returns the instance of a connected transport
    int (*connect)(void *transport, const char *addr);

    // Closes transport, it's currently required to be immediate operation
    int (*close)(void *transport);

    //  Option callbacks, only TRANSPORT-level options are passed here
    int (*set_option)(void *socket,
        int option, void *value, size_t value_len);
    int (*get_option)(void *socket,
        int option, void *value, size_t *value_len);

} xs_transport_plugin;

typedef struct xs_transport_entry {
    LIST_ENTRY (xs_transport_entry) list;
    xs_transport_plugin *plugin;
} xs_transport_entry;

typedef struct xs_transports {
    LIST_HEAD (xs_transport_list, xs_transport_entry) transports;
} xs_transports;


void xs_transport_set_data(void *transport, void *userdata);
void *xs_transport_get_socket(void *transport);
void *xs_transport_get_data(void *transport);

#endif //  XS_TRANSPORT_PLUGIN_INCLUDED
