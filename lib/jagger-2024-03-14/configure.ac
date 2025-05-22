#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.71])
AC_INIT([jagger], [2024-03-14], [ynaga@iis.u-tokyo.ac.jp])
AM_INIT_AUTOMAKE([-Wall foreign])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_AWK
AC_PROG_MAKE_SET
AC_PROG_INSTALL
AC_PROG_MKDIR_P

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_PATH_PROG(CURL,   curl,   no)
AC_PATH_PROG(WGET,   wget,   no)
AC_PATH_PROG(ICONV,  iconv,  no)
AC_PATH_PROG(PYTHON3, python3, no)
AC_CHECK_FUNCS([memchr memmove memset strchr strpbrk strtol])

JUMAN_CONV="${PYTHON3} \$(abs_top_srcdir)/tools/juman2jagger.py"
MECAB_CONV="${AWK} '{ printf \"%s\", (\$\$1 == \"EOS\") ? \"\n\" : \$\$1 }'"

AC_SUBST(JUMAN_CONV)
AC_SUBST(MECAB_CONV)

# training corpus (default model)
AC_ARG_WITH([corpus],
        [AS_HELP_STRING([--with-corpus=[corpus]],
            [set the training corpus (kyoto|kwdlc|kyoto+kwdlc)])],
        [CORPUS=$withval],
        [CORPUS=kwdlc])

if [test "$CORPUS" = "kyoto"] || [test  "$CORPUS" = "kwdlc"] || [test  "$CORPUS" = "kyoto+kwdlc"]; then
    NUM_POS_FIELD=4
    AC_DEFINE([USE_JUMANDIC], [1], [USE KYOTO/KWDLC for training])
    AC_DEFINE([NUM_POS_FIELD], [4], [number of POS field])
#    DICT=${PWD}/mecab-jumandic-5.1-20070304
    DICT=${PWD}/mecab-jumandic-7.0-20130310
fi

KYOTO_CORPUS_DIR=${PWD}/KyotoCorpus
KWDLC_CORPUS_DIR=${PWD}/KWDLC

JAGGER_MODEL_DIR="${libdir}/jagger/model"
JAGGER_CPPFLAGS+=" -DJAGGER_DEFAULT_MODEL=\\\"${JAGGER_MODEL_DIR}/${CORPUS}\\\""

AC_SUBST(CORPUS)
AC_SUBST(DICT)
AC_SUBST(KYOTO_CORPUS_DIR)
AC_SUBST(KWDLC_CORPUS_DIR)
AC_SUBST(NUM_POS_FIELD)
AC_SUBST(JAGGER_CPPFLAGS)

# training corpus (default model)
AC_ARG_WITH([corpus],
        [AS_HELP_STRING([--with-corpus=[corpus]],
            [set the training corpus (kyoto|kwdlc)])],
        [CORPUS=$withval],
        [CORPUS=kwdlc])

AC_ARG_ENABLE([compact-dict],
        [AS_HELP_STRING([--enable-compact-dict],
            [split morphological infomation to save space by 20% (speed -5%) (default=no)])],
        [],
        [enable_compact_dict="no"])
AS_IF([test "x$enable_compact_dict" != "xno"],
    [AC_DEFINE([USE_COMPACT_DICT], [1], [split morphological infomation to save space by 20% (speed -5%)])])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 tools/Makefile])
AC_OUTPUT
