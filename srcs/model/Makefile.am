# $Id: Makefile.am,v 1.1.1.1 2011-03-25 02:18:46 okamoto Exp $
bin_PROGRAMS = sigrunac sigend sigstart
bin_SCRIPTS = siggenac
SRCS = ControllerLib.cpp
sigrunac_SOURCES = $(SRCS) runmain.cpp
sigend_SOURCES = sigend.cpp
sigstart_SOURCES = sigstart.cpp

#DEFINE = -DUSE_ODE -DdDOUBLE -D CONTROLLER
DEFINE = -DUSE_ODE -DdDOUBLE -D CONTROLLER -D_DEBUG
INCLUDE = -I./commonlib
CFLAGS = -g -Wall -pipe
#CFLAGS = -pipe
CXXFLAGS = $(CFLAGS) $(INCLUDE) $(DEFINE)

COMMON_LDFLAGS = -L./commonlib -lsigverse-common

sigrunac_LDFLAGS = -rdynamic 
sigrunac_LDADD = $(COMMON_LDFLAGS)  -ldl -lode  
sigstart_LDADD = $(COMMON_LDFLAGS)

AM_MAKEFLAGS = "DEFINE=$(DEFINE)" "CFLAGS=$(CFLAGS)"
SUBDIRS=commonlib samples include
