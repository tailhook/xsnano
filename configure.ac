AC_PREREQ([2.59])
AC_INIT([libxs], [0.0.0])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([1.10 -Wall no-define])

AC_PROG_CC

AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

AC_CHECK_LIB([pthread], [pthread_create])

AC_SUBST([LIBXS_VERSION], [0:0:0])

AC_CONFIG_FILES([
    Makefile
    src/Makefile
    tests/Makefile
])

AC_OUTPUT
