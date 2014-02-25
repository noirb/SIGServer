bin_PROGRAMS = sigjsp #recogTest jspTest
bin_SCRIPTS = sigjsp.sh
sigetc_DATA = sigjsp.conf

sigjsp_SOURCES = julius_sp.cpp Jsp.cpp JspSocket.cpp JspPacket.cpp JspRecog.cpp JspWave.cpp JspXMLParser.cpp JspStringUtil.cpp JspConfigFile.cpp JspLog.cpp
#recogTest_SOURCES = recogTest.cpp JspSocket.cpp JspLog.cpp JspRecog.cpp JspWave.cpp JspXMLParser.cpp JspStringUtil.cpp JspPacket.cpp
#jspTest_SOURCES = jspTest.cpp JspTestClient.cpp JspSocket.cpp JspConfigFile.cpp JspPacket.cpp JspWave.cpp JspStringUtil.cpp JspLog.cpp

INCLUDE = -I./ -I./commonlib
CXXFLAGS = $(INCLUDE)

sigjsp_LDADD = -L./ -L./commonlib -lxerces-c -lirwas-common
#recogTest_LDADD = -L./ -lxerces-c
#jspTest_LDADD = -L./ -L./commonlib -lxerces-c -lirwas-common

AM_MAKEFLAGS = "DEFINE=$(DEFINE)" "CFLAGS=$(CFLAGS)"
SUBDIRS = commonlib
