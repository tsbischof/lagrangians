AC_PREREQ([2.69])
AC_INIT([lagrangians], [2.0.0], [tsbischof@gmail.com])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([foreign -Wall -Werror])

# Checks for programs.
AC_PROG_CXX

# Checks for libraries.
AX_BOOST_BASE([1.48],, [AC_MSG_ERROR([lagrangians needs Boost, but it was not found in your system])])
AX_BOOST_FILESYSTEM
AX_BOOST_PROGRAM_OPTIONS
AX_BOOST_REGEX
AX_BOOST_SYSTEM

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CONFIG_FILES([GNUmakefile src/GNUmakefile])

AC_OUTPUT
