#
# $Id: Makefile,v 1.2 2011-05-12 08:33:55 msi Exp $
RM = rm -f
LD = c++


# INCLUDE = -I../ -I../../ -I../../../commonlib -I../../../x3d/parser/cpp/X3DParser
INCLUDE = -I../ -I../../ -I../../../commonlib -I../../../x3d/parser/cpp/X3DParser -I/usr/java/latest/include -I/usr/java/latest/include/linux


DEFINE = -D USE_ODE -D EXEC_SIMULATION -D USE_XERCES
DEFINE += -D COMM_NO_SEND
#DEFINE += -D CONF_DUMP
DEBUG = -g	-DUNIT_TEST	 -pipe

CFLAGS = $(DEFINE) $(INCLUDE) $(DEBUG)

# ************************************************************
#  set path for libjvm.so
# ************************************************************
# JRELIBDIR = `java-config -o`/jre/lib/amd64/server
JRELIBDIR = /usr/java/latest/jre/lib/i386/server

# ************************************************************
#  set path for libcppunit.so
# ************************************************************
COMMON_LIB_PATH = /usr/local/lib

LDFLAGS = -lcppunit -lxerces-c -lode -L ../../../x3d/parser/cpp/X3DParser -lx3dc++ -L $(JRELIBDIR) -ljvm

.SUFFIXES: .cpp .o

.cpp.o:
	$(CXX) -o $@ -c $(CFLAGS) $<


TEST_SRCS = entity.cpp world.cpp fdb.cpp VectorMath.cpp SSimObjBuilderTest.cpp SPartsTest.cpp

COMMLIB = ../../../commonlib
COMM_SRCS = $(COMMLIB)/SimObj.cpp $(COMMLIB)/binary.cpp $(COMMLIB)/Attribute.cpp $(COMMLIB)/Rotation.cpp $(COMMLIB)/Parts.cpp $(COMMLIB)/Parts_Binary.cpp $(COMMLIB)/Value.cpp $(COMMLIB)/Vector3d.cpp $(COMMLIB)/Logger.cpp $(COMMLIB)/SimWorld.cpp $(COMMLIB)/SimObjBase.cpp $(COMMLIB)/CParts.cpp
SERVER_SRCS = ../../SSimObj.cpp ../../Joint.cpp	 ../../ODEWorld.cpp ../../SSimWorld.cpp ../../SParts.cpp
READCONF_SRCS = ../BodyXMLReader.cpp ../TransformStack.cpp ../XMLUtils.cpp ../Eval.cpp ../infix2postfix.cpp ../rpnCalc.cpp ../JointParser.cpp ../SegmentParser.cpp ../GeometryParser.cpp ../WorldXMLReader.cpp ../EntityXMLReader.cpp ../SSimObjBuilder.cpp ../X3DFileReader.cpp ../SgvX3DSimObjCreator.cpp ../FilenameDB.cpp ../SimpleShapeDB.cpp ../SimpleShapeXMLReader.cpp
LIB_SRCS = $(READCONF_SRCS) $(COMM_SRCS) $(SERVER_SRCS)

SRCS = $(TEST_SRCS) $(LIB_SRCS)

TEST_OBJS = $(TEST_SRCS:.cpp=.o)
LIB_OBJS = $(LIB_SRCS:.cpp=.o) ../../fname.o ../../jjudge.o

TESTS = $(TEST_SRCS:.cpp=)

OBJS = $(SRCS:.cpp=.o)

#TARGET = VectorMath
#TARGET = SSimObjBuilderTest
TARGET = SPartsTest

target:
	$(MAKE) compile "TEST_OBJ=$(TARGET).o"
	LD_LIBRARY_PATH=$(COMMON_LIB_PATH):$(JRELIBDIR) ./$(TARGET)

all:
	for i in $(TESTS); \
	do ( $(MAKE) compile "TEST_OBJ=$$i.o") \
	done
	$(MAKE) run

compile: $(LIB_OBJS) $(TEST_OBJ)
	$(LD) -o $(TEST_OBJ:.o=) $(TEST_OBJ) $(LIB_OBJS) $(LDFLAGS)

run :
	@echo run tests
	@for i in $(TESTS); \
	do ( \
		if [ -e $$i ]; then \
			export SIGVERSE_X3DPARSER_CONFIG="../../../server/X3DParser.cfg"; \
			LD_LIBRARY_PATH=$(COMMON_LIB_PATH):$(JRELIBDIR) ./$$i; \
		fi \
	) done

../../fname.o: ../../fname.c
../../jjudge.o: ../../jjudge.c

distclean: clean

clean:
	$(RM) $(TESTS) $(OBJS) *~

