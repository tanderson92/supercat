AC_INIT(supercat, 0.5.6, bug-spc@nosredna.net)
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_LANG([C])
AC_PROG_CC
AC_C_CONST
AC_HEADER_STDC
AC_STRUCT_TM
AC_FUNC_STAT
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset regcomp strchr strstr strerror])
AC_CHECK_HEADERS([unistd.h sys/stat.h time.h stdio.h stdlib.h sys/types.h regex.h getopt.h string.h stdarg.h errno.h])

#Where do we put our spcrc files?
AC_MSG_CHECKING([where to install the spcrc files])
AC_ARG_WITH([system-directory],
    AS_HELP_STRING([--with-system-directory=dir], [Location of installation of supercat config files (default: /etc)]),
    SYSTEM_DIRECTORY=`eval echo $withval`,
    SYSTEM_DIRECTORY=`eval echo /etc/supercat`)
AC_MSG_RESULT([$SYSTEM_DIRECTORY])
AC_SUBST([SYSTEM_DIRECTORY])
#

AM_PROG_INSTALL
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile doc/Makefile spcrc/Makefile])
AC_OUTPUT
AC_CONFIG_SRCDIR([src/])
