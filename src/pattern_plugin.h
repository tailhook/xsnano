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

#ifndef XS_PATTERN_PLUGIN_INCLUDED
#define XS_PATTERN_PLUGIN_INCLUDED

#include <stdlib.h>
#include <sys/queue.h>
#include "msg.h"


typedef struct xs_pattern_plugin {
    //  base plugin
    int type;       //  = XS_PLUGIN_PATTERN
    int version;    //  = 1
    //  end of base plugin

    //  Socket type which is specified at socket creation time e.g. XS_REQ
    int socket_type;
    //  PATTERN, which is sent in the SP header in the wire
    int pattern;
    //  ROLE, which is sent in the SP header in the wire
    int role;

    //  Socket initialization
    int (*init_socket)(void *socket);
    //  Start socket shutdown, more callbacks may be needed
    int (*close_socket)(void *socket);

    //  Option callbacks, only PATTERN-level options are passed here
    int (*set_option)(void *socket,
        int option, void *value, size_t value_len);
    int (*get_option)(void *socket,
        int option, void *value, size_t *value_len);

    //  Callback which called to check capabilies of transport before doing
    //  connect or bind
    int (*check_transport)(void *socket, void *transport);
    //  Callback to add the real transport
    int (*add_stream)(void *socket, void *stream);

    int (*send)(void *socket, xs_msg *msg, int flags);
    int (*recv)(void *socket, xs_msg *msg, int flags);

} xs_pattern_plugin;

typedef struct xs_pattern_entry {
    LIST_ENTRY (xs_pattern_entry) list;
    xs_pattern_plugin *plugin;
} xs_pattern_entry;

typedef struct xs_patterns {
    LIST_HEAD (xs_pattern_list, xs_pattern_entry) patterns;
} xs_patterns;


void xs_pattern_set_data(void *socket, void *userdata);
void *xs_pattern_get_data(void *socket);

#endif //  XS_PATTERN_PLUGIN_INCLUDED
