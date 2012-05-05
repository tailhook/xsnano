lib_LTLIBRARIES = libxsnano.la

include_HEADERS = ../include/xs.h

libxsnano_la_SOURCES = \
    xs.c \
    ctx.h ctx.c \
    err.h err.c \
    likely.h \
    mutex.h mutex.c \
    pub.h pub.c \
    sock.h sock.c \
    sub.h sub.c \
    xpub.h xpub.c \
    xsub.h xsub.c

libxsnano_la_LDFLAGS = -version-info $(XSNANO_VERSION)

