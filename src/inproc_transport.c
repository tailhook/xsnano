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

    Implementation details:

    * We curently provision bound sockets if connect() executed before bind()
    * The search for bound socket is linear


*/

#include <sys/queue.h>
#include <errno.h>
#include <string.h>

#include "plugin.h"
#include "transport_plugin.h"
#include "inproc_transport.h"
#include "inproc_stream.h"
#include "stream.h"
#include "mutex.h"
#include "err.h"

#define INPROC_CONNECTED 1
#define INPROC_BOUND 2

typedef struct inproc_connected {
    int type;
    LIST_ENTRY(inproc_connected) list;
    void *stream;
    void *transport;
} inproc_connected;

typedef struct inproc_bound {
    int type;
    LIST_ENTRY(inproc_bound) list;
    void *transport;
    LIST_HEAD(connected_sockets, inproc_connected) connected;
    char addr[];
} inproc_bound;

static struct {
    xs_mutex mutex;
    LIST_HEAD(bound_sockets, inproc_bound) bound_sockets;
} registry;


void init_bound_socket(inproc_bound *bound, const char *addr) {
    bound->type = INPROC_BOUND;
    bound->transport = NULL;
    strcpy(bound->addr, addr);
    LIST_INIT(&bound->connected);
    LIST_INSERT_HEAD(&registry.bound_sockets, bound, list);
}

static int connect_sockets (inproc_bound *bound, inproc_connected *conn) {
    int rc;
    xs_inproc_stream *s1 = malloc (sizeof (xs_inproc_stream));
    if (!s1)
        return -ENOMEM;
    rc = xs_msg_pipe_init (&s1->inpipe);
    if (rc < 0)
        goto err_s1_pipe;
    xs_inproc_stream *s2 = malloc (sizeof (xs_inproc_stream));
    if (!s2) {
        rc = -ENOMEM;
        goto err_s2;
    }
    rc = xs_msg_pipe_init (&s2->inpipe);
    if (rc < 0)
        goto err_s2_pipe;
    s1->outpipe = &s2->inpipe;
    s2->outpipe = &s1->inpipe;
    s1->otherstream = s2;
    s2->otherstream = s1;
    void *ss1;
    rc = xs_stream_create(&ss1);
    if (rc < 0)
        goto err_ss1;
    void *ss2;
    rc = xs_stream_create(&ss2);
    if (rc < 0)
        goto err_ss2;
    xs_stream_set_plugin(ss1, &xs_inproc_stream_plugin);
    xs_stream_set_data(ss1, s1);
    xs_stream_set_plugin(ss2, &xs_inproc_stream_plugin);
    xs_stream_set_data(ss2, s2);
    rc = xs_socket_add_stream(xs_transport_get_socket(bound->transport), ss1);
    if (rc < 0)
        goto err_add_ss1;
    rc = xs_socket_add_stream(xs_transport_get_socket(conn->transport), ss2);
    if (rc < 0)
        goto err_add_ss2;
    return 0;
err_add_ss2:
    xs_assert (0); // find a way to remove stream from socket
err_add_ss1:
    xs_stream_term(ss2);
err_ss2:
    xs_stream_term(ss1);
err_ss1:
    xs_msg_pipe_free(&s2->inpipe);
err_s2_pipe:
    free(s2);
err_s2:
    xs_msg_pipe_free(&s1->inpipe);
err_s1_pipe:
    free(s1);
    return rc;
}

static int inproc_bind(void *transport, const char *addr) {
    int rc;
    int addrlen = strlen(addr);
    xs_mutex_lock(&registry.mutex);
    inproc_bound *bound;
    LIST_FOREACH(bound, &registry.bound_sockets, list) {
        if(!strcmp(bound->addr, addr)) {
            if(bound->transport) {
                xs_mutex_unlock(&registry.mutex);
                return -EADDRINUSE;
            } else {
                bound->transport = transport;
                // Notify connected sockets
                inproc_connected *conn;
                LIST_FOREACH(conn, &bound->connected, list) {
                    rc = connect_sockets(bound, conn);
                    if (rc < 0) {
                        xs_assert(0);
                        // TODO(tailhook):
                        // * disconnect just connected sockets
                        // * return failure to user
                    }
                }
                xs_transport_set_data(transport, bound);
                xs_mutex_unlock(&registry.mutex);
                return 0;
            }
        }
    }
    bound = malloc(sizeof(inproc_bound) + addrlen + 1);
    if(!bound) {
        xs_mutex_unlock(&registry.mutex);
        return -ENOMEM;
    }
    init_bound_socket(bound, addr);
    bound->transport = transport;
    xs_transport_set_data(transport, bound);
    xs_mutex_unlock(&registry.mutex);
    return 0;
}


static int inproc_connect(void *transport, const char *addr) {
    int rc;
    xs_mutex_lock(&registry.mutex);
    inproc_bound *bound;
    LIST_FOREACH(bound, &registry.bound_sockets, list) {
        if(!strcmp(bound->addr, addr)) {
            break;
        }
    }
    if(!bound) { //  No bound socket? Let's provision one
        int addrlen = strlen(addr);
        bound = malloc(sizeof(inproc_bound) + addrlen + 1);
        if(!bound) {
            xs_mutex_unlock(&registry.mutex);
            return -ENOMEM;
        }
        init_bound_socket(bound, addr);
    }
    inproc_connected *conn = malloc(sizeof(inproc_connected));
    if(!conn) {
        xs_mutex_unlock(&registry.mutex);
        return -ENOMEM;
    }
    conn->type = INPROC_CONNECTED;
    conn->transport = transport;
    LIST_INSERT_HEAD(&bound->connected, conn, list);

    if(bound->transport) {
        rc = connect_sockets(bound, conn);
        if (rc < 0) {
            LIST_REMOVE(conn, list);
            free(conn);
            xs_mutex_unlock(&registry.mutex);
            return rc;
        }
    }

    xs_transport_set_data(transport, conn);
    xs_mutex_unlock(&registry.mutex);
    return 0;
}

static xs_transport_plugin xs_inproc_transport_struct = {
    /* type */ XS_PLUGIN_TRANSPORT,
    /* version */ 1,
    /* name */ "inproc",
    /* bind */ inproc_bind,
    /* connect */ inproc_connect,
    /* set_option */ NULL,
    /* get_option */ NULL
};

xs_base_plugin *xs_inproc_transport() {
    xs_mutex_init(&registry.mutex);
    LIST_INIT(&registry.bound_sockets);
    return (xs_base_plugin*)&xs_inproc_transport_struct;
}
