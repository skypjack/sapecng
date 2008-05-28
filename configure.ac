#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([sapec-ng],[1.9.0],[skypjack@gmail.com])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_SRCDIR([src/sapec-ng.c])
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC
AM_PROG_LEX
AC_PROG_YACC

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h strings.h unistd.h])

# Checks for gtk2
PKG_CHECK_MODULES([GTK2],[gtk+-2.0],gtk2="yes",AC_MSG_WARN(GTK2_PKG_ERRORS))
AM_CONDITIONAL(GTK2_H, test x$gtk2 = xyes)
AC_SUBST([GTK2_CFLAGS])
AC_SUBST([GTK2_LIBS])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([bzero])

# AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([Makefile
		 src/Makefile])
AC_OUTPUT
