# $Id: Makefile.am,v 1.3 2011-05-12 08:33:53 msi Exp $
bin_PROGRAMS = sigserver
bin_SCRIPTS = sigserver.sh sigkill.sh sigcreate.sh
sigserver_SOURCES = SSimObj.cpp \
	Joint.cpp \
	ODEWorld.cpp \
	SParts.cpp \
	ServerAcceptProc.cpp \
	LogTransfer.cpp \
	SSimWorld.cpp \
	PacketForward.cpp \
	WorldSimulator.cpp \
	ServiceNameServer.cpp \
	simserver.cpp fname.c jjudge.c \
	SSimEntity.cpp


fname.o : fname.c

jjudge.o : jjudge.c

sigetc_DATA = X3DParser.cfg

# -----------------------------------------------------
# Release mode setting
#DEFINE = -D IRWAS_SIMSERVER -D dDOUBLE -D STRICT_INTERFACE
#CFLAGS = -pipe
# -----------------------------------------------------
# Debug mode setting
DEFINE = -D IRWAS_SIMSERVER -D dDOUBLE -D STRICT_INTERFACE -D _DEBUG
CFLAGS = -pipe -g -Wall
# -----------------------------------------------------

PWD_INCLUDE = -I ./commonlib -I ./readconf
CXXFLAGS = $(CFLAGS) $(DEFINE) $(PWD_INCLUDE)
X3DCPPDIR = X3DParser
sigserver_LDADD = -lirwas-common -L ./commonlib -lreadconf -L ./readconf -lx3dc++ -L $(X3DCPPDIR) -ljvm -L $(LIBJVMDIR)
SUBDIR_INCLUDE=-I ./ -I ../commonlib -I ../ -I ../../../ -I ../$(X3DCPPDIR)
AM_MAKEFLAGS = "CFLAGS=$(CFLAGS)" "INCLUDE=$(SUBDIR_INCLUDE)" "DEFINE=$(DEFINE)"
SUBDIRS = readconf commonlib X3DParser conf



