AC_PREREQ([2.63])
AC_INIT([xpi], [4.0.0], [servicedesk@fedict.be])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([src/content/belgiumeidpkcs11.js])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE
AC_CANONICAL_HOST

# Checks for programs.
# Check for zip
AC_PATH_PROG([ZIP], [zip])
AC_PROG_SED

############################################################################
# Test for platform and set corresponding AM_CONDITIONALs for Makefile.am  #
############################################################################

linux="no"
osx="no"
win32="no"

AS_CASE([$host],
  [*-*-linux*],[linux=true],
  [*-*-darwin*],[osx=true],
  [*-mingw*|*-winnt*],[win32=true])

AM_CONDITIONAL([LINUX],[test x$linux = xtrue])
AM_CONDITIONAL([OSX],[test x$osx = xtrue])
AM_CONDITIONAL([WIN32],[test x$win32 = xtrue])

# For now no make install on Windows
#programfiles_86_exists=no
#programfiles_exists=no
#AS_VAR_SET_IF([PROGRAMW6432],[programfiles_86_exists=true])
#AM_CONDITIONAL([PROGRAMFILES_86_EXISTS],[test x$programfiles_86_exists = xtrue])
#AS_VAR_SET_IF([PROGRAMFILES],[programfiles_exists=true])
#AM_CONDITIONAL([PROGRAMFILES_EXISTS],[test x$programfiles_exists = xtrue])

# Checks for libraries.

dnl this macro is used to get the arguments supplied
dnl to the configure script (./configure --enable-debug)
dnl Check if we have enable debug support.
AC_MSG_CHECKING(whether to enable debugging)
default="no"
DEBUG=
DEBUG_POSTFIX=
AC_ARG_ENABLE(debug, [  --enable-debug=[no/yes] turn on debugging
	                       [default=$default]],, enable_debug=$default)
if test "x$enable_debug" = "xyes"; then
	         CFLAGS="$CFLAGS -g -D_DEBUG -Wall -O0"
	         DEBUG=-g
	         DEBUG_POSTFIX=_debug;
AC_MSG_RESULT(yes)
else
AC_MSG_RESULT(no)
fi

AC_ARG_WITH([mozext],
	  AS_HELP_STRING([--with-mozext=DIR],[Mozilla extension directory]),
	  [],
	  [with_mozext=no])

AS_IF(
	[test "x$with_mozext" = "xno"],
	[AM_COND_IF(
		[LINUX],
		[mozext]=[${datadir}/mozilla/extensions/{ec8030f7-c20a-464f-9b0e-13a3a9e97384}],
		[AM_COND_IF(
			[OSX],
			[mozext]=[/Library/Application\ Support/Mozilla/Extensions/{ec8030f7-c20a-464f-9b0e-13a3a9e97384}],
			[mozext]=[${datadir}]
		)]
	)],
	[[mozext]=[$with_mozext]]
)

AC_SUBST([ZIP])
AC_SUBST([DEBUG])
AC_SUBST([DEBUG_POSTFIX])
AC_SUBST([mozext])

# For now no make install on Windows
#AC_SUBST([PROGRAMW6432])
#AC_SUBST([PROGRAMFILES])

# Checks for library functions.

AC_CONFIG_FILES([
	Makefile
	src/Makefile
])
AC_OUTPUT
