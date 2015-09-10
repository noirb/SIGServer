DEPFILE = Makefile.dep

CXXFLAGS = $(CFLAGS)
#INCLUDE = -I ../../commonlib -I ./ -I ../
#DEFINE = -D USE_ODE -D dDOUBLE -D USE_XERCES
# DEFINE += -D BodyXMLReader_test
# DEFINE += -D EntityXMLReader_test -D NO_WORLD

# LDFLAGS = -lxerces-c -lode

.SUFFIXES: .cpp .o

.cpp.o:
	$(CXX) $(CXXFLAGS) $(DEFINE) $(INCLUDE) -c -o $@ $<

ARCHIVE = libreadconf.a


# FIX20110421(ExpSS)
# SRCS = JointParser.cpp SegmentParser.cpp GeometryParser.cpp BodyXMLReader.cpp EntityXMLReader.cpp TransformStack.cpp XMLUtils.cpp WorldXMLReader.cpp Eval.cpp infix2postfix.cpp rpnCalc.cpp SgvX3DSimObjCreator.cpp X3DFileReader.cpp SSimObjBuilder.cpp FilenameDB.cpp
SRCS = JointParser.cpp SegmentParser.cpp GeometryParser.cpp BodyXMLReader.cpp EntityXMLReader.cpp TransformStack.cpp XMLUtils.cpp WorldXMLReader.cpp Eval.cpp infix2postfix.cpp rpnCalc.cpp SgvX3DSimObjCreator.cpp X3DFileReader.cpp SSimObjBuilder.cpp FilenameDB.cpp SimpleShapeXMLReader.cpp SimpleShapeDB.cpp ShapeFileReader.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(ARCHIVE)

$(ARCHIVE) : $(OBJS)
	ar -cr $(ARCHIVE) $(OBJS)

distclean: clean

clean:
	$(RM) *.o *~ $(ARCHIVE)

# for autoconf
install:

uninstall:

depend:

	gcc -MM $(DEFINE) $(INCLUDE) $(SRCS) > $(DEPFILE)

include $(DEPFILE)
