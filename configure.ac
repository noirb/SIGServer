#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
#
# Modified by Tetsunari Inamura on 2015-04-23

AC_PREREQ(2.59)
AC_INIT(SIGVerse, 2015-04-23, BUG-REPORT-ADDRESS)
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_SRCDIR([srcs])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
#AC_PROG_LD
AC_PROG_LN_S

SIGVERSE_VERSION=2.3.0
test "$prefix" = NONE && prefix=$HOME/sigverse-$SIGVERSE_VERSION
bindir="$prefix/bin"
includedir="$prefix/include"
sigincludedir="$includedir/sigverse"
datadir="${prefix}/share/sigverse"
sigetcdir="$datadir/etc"
sigdatadir="$datadir/data"
sigdataxmldir="$datadir/data/xml"
sigdatashapedir="$datadir/data/shape"
jardir="$datadir/jar"
samplesdir="$datadir/samples"

AC_SUBST(sigetcdir)
AC_SUBST(sigincludedir)
AC_SUBST(sigdatadir)
AC_SUBST(sigdataxmldir)
AC_SUBST(sigdatashapedir)
AC_SUBST(jardir)
AC_SUBST(confdir)
AC_SUBST(samplesdir)

X3DPARSER_CFG="$sigetcdir/X3DParser.cfg"
AC_SUBST(X3DPARSER_CFG)


# Checks for libraries.
AC_CHECK_LIB([xerces-c], [main])

# FIXME: Replace `main' with a function in `-lode':
AC_CHECK_LIB([ode], [main])

# FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread], [main])

# FIXME: Replace `main' with a function in `-lpthread':
#AC_CHECK_LIB([jvm], [main])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

#
# for julius
#

AC_ARG_WITH(julius,
	AC_HELP_STRING([--with-julius=@<:@juliusbin@:>@], [julius executable]),
	juliusbin="$withval", juliusbin="")
AC_SUBST(juliusbin)

AC_ARG_WITH(dictkit,
	AC_HELP_STRING([--with-dictkit=@<:@dictkitpath@:>@], [julius dictation kit directory]),
	dictkitdir="$withval", dictkitdir="")
AC_SUBST(dictkitdir)

juliusbin="/usr/bin"
tmp_dictkitdir=`find /usr/local -name "fast.jconf" | sed -n '$p'`
dictkitdir=${tmp_dictkitdir%/*}

if ! test -z $juliusbin
then
	AC_CHECK_FILE($juliusbin,,AC_MSG_ERROR(Cannot find julius executable. Use --with-julius option))
	if ! test -z $dictkitdir
	then
		AC_CHECK_FILE("$dictkitdir/fast.jconf",,AC_MSG_ERROR(Cannot find julius dictation kit. Use --with-dictkit option))
		AC_CHECK_FILE("$dictkitdir/model",,AC_MSG_ERROR(Cannot find julius dictation kit. Use --with-dictkit option))
	fi

fi


#
# for jdk libjvm.so
#
AC_ARG_WITH(jvm,
	AC_HELP_STRING([--with-jvm=@<:@PATH@:>@], [jdk path for libjvm.so(ex. <jdkdir>/jre/lib/<arch>/server)]),
	LIBJVMDIR="$withval", LIBJVMDIR=="")

tmp=`find /usr/lib/jvm/ -name "libjvm.so" | grep server/libjvm.so | sed -n '$p'`
LIBJVMDIR=${tmp%/*}

AC_CHECK_FILE("$LIBJVMDIR/libjvm.so",, AC_MSG_ERROR(Cannot find libjvm.so.))

#LIBJVMDIR="/usr/lib/jvm/java-1.6.0-openjdk-i386/jre/lib/i386/server"
AC_SUBST(LIBJVMDIR)


AC_CONFIG_FILES([
		Makefile
		jar/Makefile
		srcs/Makefile
		srcs/server/Makefile
		srcs/server/sigserver.sh
		srcs/server/X3DParser.cfg
		srcs/server/conf/Makefile
		srcs/model/Makefile
		srcs/model/siggenac
		srcs/model/include/Makefile
		srcs/model/samples/export/Makefile
		srcs/model/samples/export/Makefile.controller
		srcs/julius-sp/Makefile
		srcs/julius-sp/sigjsp.sh
		srcs/julius-sp/sigjsp.conf
		])
             
AC_OUTPUT
