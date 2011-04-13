dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(javascriptcore-gobject, 1.0)


AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE()

AC_PROG_CC
AC_PROG_LIBTOOL
PKG_PROG_PKG_CONFIG



PKG_CHECK_MODULES(DEPENDENCIES, [webkitgtk-3.0 >= 1.3.13
								 gobject-2.0
								 glib-2.0
])

AC_CONFIG_FILES(Makefile src/Makefile data/Makefile data/javascriptcore-gobject-1.0.pc)
AC_OUTPUT

