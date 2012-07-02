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

#include "pattern_plugin.h"
#include "sock.h"
#include "ctx.h"

void xs_pattern_set_data(void *socket, void *userdata) {
    ((xs_sock *)socket)->pattern_data = userdata;
}

void *xs_pattern_get_data(void *socket) {
    return ((xs_sock *)socket)->pattern_data;
}

int xs_plug_pattern (xs_ctx *ctx, xs_pattern_plugin *plugin) {
    xs_pattern_entry *entry;
    LIST_FOREACH (entry, &ctx->patterns.patterns, list) {
        if (entry->plugin->socket_type == plugin->socket_type) {
            return -EBUSY;
        }
    }
    entry = malloc (sizeof (xs_pattern_entry));
    if(!entry)
        return -ENOMEM;
    entry->plugin = plugin;
    LIST_INSERT_HEAD (&ctx->patterns.patterns, entry, list);
    return 0;
}

int xs_patterns_init (xs_ctx *ctx) {
    LIST_INIT (&ctx->patterns.patterns);
    return 0;
}

int xs_patterns_free (xs_ctx *ctx) {
    xs_pattern_entry *cur, *next;
    for (cur = LIST_FIRST (&ctx->patterns.patterns); cur; cur=next) {
        next = LIST_NEXT (cur, list);
        free (cur);
    }
    return 0;
}

int xs_init_pattern(xs_sock *socket) {
    xs_pattern_entry *entry;
    LIST_FOREACH (entry, &socket->ctx->patterns.patterns, list) {
        if (entry->plugin->socket_type == socket->type) {
            socket->pattern = entry->plugin;
            return entry->plugin->init_socket(socket);
        }
    }
    return -ENOTSUP;
}
