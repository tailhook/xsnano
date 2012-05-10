lib_LTLIBRARIES = libxs.la

include_HEADERS = ../include/xs.h

libxs_la_SOURCES = \
    xs.c \
    ctx.h ctx.c \
    err.h err.c \
    instream.h instream.c \
    likely.h \
    msg.h msg.c \
    mutex.h mutex.c \
    outstream.h outstream.c \
    pub.h pub.c \
    sock.h sock.c \
    sub.h sub.c \
    xpub.h xpub.c \
    xsub.h xsub.c

libxs_la_LDFLAGS = -version-info $(LIBXS_VERSION)

