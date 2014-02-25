# $Id: Makefile,v 1.3 2011-04-05 09:06:19 kawamoto Exp $
DEPFILE = Makefile.dep

AR = ar -cr
CXX = g++
DEFINE = -DUSE_ODE -DdDOUBLE -D_DEBUG
#DEFINE = -DUSE_ODE -DdDOUBLE
INCLUDE = -I ./
#CXXFLAGS = -Wall -g
CXXFLAGS = $(CFLAGS) $(DEFINE) $(INCLUDE)

SUBDIRS = comm comm/encoder comm/event comm/controller command ct

SUBDIRS_OBJS = comm/encoder/*.o comm/event/*.o comm/controller/*.o comm/*.o command/*.o ct/*.o

ARCHIVE_A = libirwas-common.a
ARCHIVE_SO = libirwas-common.so

SRCS = Attribute.cpp \
	CommDataDecoder.cpp \
	CommUtil.cpp \
	Parts.cpp \
	CParts.cpp \
	SimObjBase.cpp \
	SimObj.cpp \
	SimWorld.cpp \
	Value.cpp \
	binary.cpp \
	Rotation.cpp \
	Logger.cpp \
	Vector3d.cpp \
	Parts_Binary.cpp \
	ControllerImpl.cpp \
	ViewImage.cpp \
	RawSound.cpp \
	MessageSender.cpp \
	Source.cpp \
	Controller.cpp

OBJS = $(SRCS:.cpp=.o)

.SUFFIXES: .cpp .o

.cpp.o:
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all:
	$(MAKE) $(ARCHIVE_A)
#	make clean-objs
#	make $(ARCHIVE_SO)


lib : $(OBJS) subdirs
	$(AR) $(ARCHIVE) $(OBJS) $(SUBDIRS_OBJS)

subdirs :
	@for i in $(SUBDIRS); \
	do ( pushd ./; cd $$i; $(MAKE) "CXXFLAGS=$(CXXFLAGS)" "DEFINE=$(DEFINE)"; popd; ) \
	done


$(ARCHIVE_SO) : dummy
	$(MAKE) lib "ARCHIVE=$(ARCHIVE_SO)" "CXXFLAGS=-fPIC $(CXXFLAGS)" "AR=g++ -shared -o"

$(ARCHIVE_A) : dummy
	@echo creating $@...
	$(MAKE) lib "ARCHIVE=$(ARCHIVE_A)"

dummy:

clean:
	@echo Cleaning...
	@for i in $(SUBDIRS); \
	do ( \
		if [ -d $$i ]; then \
			pushd ./; cd $$i; $(MAKE) clean; popd; \
		fi; \
	) done
	$(RM) *.o *.a $(ARCHIVE) *~
	@echo Done

distclean: clean

clean-objs:
	$(RM) *.o 

depend:
	@echo creating dependency...
	gcc -MM $(DEFINE) $(SRCS) $(INCLUDE) > $(DEPFILE) 
	@for i in $(SUBDIRS); \
	do \
		if [ -d $$i ]; then \
			pushd ./; cd $$i; \
			$(MAKE) depend; \
			popd; \
		fi;\
	done
	@echo Done

install:

uninstall:

include $(DEPFILE)
