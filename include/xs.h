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

#ifndef XS_H_INCLUDED
#define XS_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <errno.h>
#include <stddef.h>

/*  Handle DSO symbol visibility                                             */
#if defined _WIN32
#   if defined DLL_EXPORT
#       define XS_EXPORT __declspec(dllexport)
#   else
#       define XS_EXPORT __declspec(dllimport)
#   endif
#else
#   if defined __SUNPRO_C  || defined __SUNPRO_CC
#       define XS_EXPORT __global
#   elif (defined __GNUC__ && __GNUC__ >= 4) || defined __INTEL_COMPILER
#       define XS_EXPORT __attribute__ ((visibility("default")))
#   else
#       define XS_EXPORT
#   endif
#endif

/******************************************************************************/
/*  Crossroads versioning support.                                            */
/******************************************************************************/

/*  Version macros for compile-time API version detection                     */
#define XS_VERSION_MAJOR 1
#define XS_VERSION_MINOR 0
#define XS_VERSION_PATCH 1

#define XS_MAKE_VERSION(major, minor, patch) \
    ((major) * 10000 + (minor) * 100 + (patch))
#define XS_VERSION \
    XS_MAKE_VERSION(XS_VERSION_MAJOR, XS_VERSION_MINOR, XS_VERSION_PATCH)

/*  Run-time API version detection                                            */
XS_EXPORT void xs_version (int *major, int *minor, int *patch);

/******************************************************************************/
/*  Crossroads errors.                                                        */
/******************************************************************************/

/*  A number random enough not to collide with different errno ranges on      */
/*  different OSes. The assumption is that error_t is at least 32-bit type.   */
#define XS_HAUSNUMERO 156384712

/*  On Windows platform some of the standard POSIX errnos are not defined.    */
#ifndef ENOTSUP
#define ENOTSUP (XS_HAUSNUMERO + 1)
#endif
#ifndef EPROTONOSUPPORT
#define EPROTONOSUPPORT (XS_HAUSNUMERO + 2)
#endif
#ifndef ENOBUFS
#define ENOBUFS (XS_HAUSNUMERO + 3)
#endif
#ifndef ENETDOWN
#define ENETDOWN (XS_HAUSNUMERO + 4)
#endif
#ifndef EADDRINUSE
#define EADDRINUSE (XS_HAUSNUMERO + 5)
#endif
#ifndef EADDRNOTAVAIL
#define EADDRNOTAVAIL (XS_HAUSNUMERO + 6)
#endif
#ifndef ECONNREFUSED
#define ECONNREFUSED (XS_HAUSNUMERO + 7)
#endif
#ifndef EINPROGRESS
#define EINPROGRESS (XS_HAUSNUMERO + 8)
#endif
#ifndef ENOTSOCK
#define ENOTSOCK (XS_HAUSNUMERO + 9)
#endif
#ifndef EAFNOSUPPORT
#define EAFNOSUPPORT (XS_HAUSNUMERO + 10)
#endif

/*  Native Crossroads error codes.                                            */
#define ETERM (XS_HAUSNUMERO + 53)

/*  This function retrieves the errno as it is known to Crossroads library.   */
/*  The goal of this function is to make the code 100% portable, including    */
/*  where Crossroads are compiled with certain CRT library (on Windows) is    */
/*  linked to an application that uses different CRT library.                 */
XS_EXPORT int xs_errno (void);

/*  Resolves system errors and Crossroads errors to human-readable string.    */
XS_EXPORT const char *xs_strerror (int errnum);

/******************************************************************************/
/*  Crossroads intilisation and shutdown.                                     */
/******************************************************************************/

XS_EXPORT int xs_init (void);
XS_EXPORT int xs_term (void);

/******************************************************************************/
/*  Crossroads socket definition.                                             */
/******************************************************************************/

/*  Socket types.                                                             */
#define XS_PUB 1
#define XS_SUB 2
#define XS_XPUB 9
#define XS_XSUB 10

/*  Socket option levels.                                                     */
#define XS_SOL_SOCKET 1

/*  Socket options.                                                           */
#define XS_SUBSCRIBE 6
#define XS_UNSUBSCRIBE 7
#define XS_TYPE 16

/*  Send/recv options.                                                        */
#define XS_DONTWAIT 1

XS_EXPORT int xs_socket (int type);
XS_EXPORT int xs_close (int s);
XS_EXPORT int xs_setsockopt (int s, int level, int option, const void *optval,
    size_t optvallen); 
XS_EXPORT int xs_getsockopt (int s, int level, int option, void *optval,
    size_t *optvallen);
XS_EXPORT int xs_bind (int s, const char *addr);
XS_EXPORT int xs_connect (int s, const char *addr);
XS_EXPORT int xs_shutdown (int s, int how);
XS_EXPORT int xs_send (int s, const void *buf, size_t len, int flags);
XS_EXPORT int xs_recv (int s, void *buf, size_t len, int flags);

#undef XS_EXPORT

#ifdef __cplusplus
}
#endif

#endif

