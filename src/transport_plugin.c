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

#include <string.h>

#include "transport_plugin.h"
#include "transport.h"

void xs_transport_set_data(void *transport, void *data) {
    ((xs_transport *)transport)->plugin_data = data;
}

void *xs_transport_get_data(void *transport) {
    return ((xs_transport *)transport)->plugin_data;
}

void *xs_transport_get_socket(void *transport) {
    return ((xs_transport *)transport)->socket;
}

int xs_plug_transport (xs_ctx *ctx, xs_transport_plugin *plugin) {
    xs_transport_entry *entry;
    LIST_FOREACH (entry, &ctx->transports.transports, list) {
        if (!strcmp(entry->plugin->name, plugin->name)) {
            return -EBUSY;
        }
    }
    entry = malloc (sizeof (xs_transport_entry));
    if(!entry)
        return -ENOMEM;
    entry->plugin = plugin;
    LIST_INSERT_HEAD (&ctx->transports.transports, entry, list);
    return 0;
}

int xs_transports_init (xs_ctx *ctx) {
    LIST_INIT (&ctx->transports.transports);
    return 0;
}

int xs_transports_free (xs_ctx *ctx) {
    xs_transport_entry *cur, *next;
    for (cur = LIST_FIRST (&ctx->transports.transports); cur; cur=next) {
        next = LIST_NEXT (cur, list);
        free (cur);
    }
    return 0;
}

int xs_create_transport (xs_ctx *ctx, xs_transport **trans, const char *addr) {
    char *addrend = strstr(addr, "://");
    if (!addrend)
        return -EINVAL;
    int addrlen = addrend - addr;

    xs_transport *tr = malloc (sizeof (xs_transport));
    if (!tr)
        return -ENOMEM;
    tr->context = ctx;
    tr->socket = NULL;
    xs_transport_entry *entry;

    LIST_FOREACH (entry, &ctx->transports.transports, list) {
        if (!strncmp(entry->plugin->name, addr, addrlen)
            && entry->plugin->name[addrlen] == 0) {

            xs_transport_plugin *plug = entry->plugin;
            tr->plugin = plug;
            *trans = tr;

            return 0;
        }
    }
    free(tr);
    return -ENOTSUP;
}

