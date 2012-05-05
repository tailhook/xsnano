AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}
lib_LTLIBRARIES = libxsnano.la

libxsnano_la_SOURCES = \
    include/xs.h src/xs.c\
    src/ctx.h src/ctx.c \
    src/err.h src/err.c \
    src/likely.h \
    src/mutex.h src/mutex.c \
    src/sock.h src/sock.c

libxsnano_la_LDFLAGS = -version-info $(XSNANO_VERSION)

dist_noinst_SCRIPTS = autogen.sh
