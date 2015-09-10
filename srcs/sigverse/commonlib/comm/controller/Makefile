DEPFILE = Makefile.dep

INCLUDE_LOCAL =  -I ../../ -I ../

.SUFFIXES: .cpp .o

.cpp.o :
	$(CXX) $(CXXFLAGS) $(INCLUDE_LOCAL) $(INCLUDE) $(DEFINE) -c -o $@ $<


SRCS = Action.cpp \
	RecvText.cpp \
	RecvSound.cpp \
	RecvMessage.cpp \
	Collision.cpp

OBJS = $(SRCS:.cpp=.o)


all: $(OBJS)

clean:
	$(RM) *.o *~ core

depend:
	gcc -Wall -MM $(INCLUDE_LOCAL) $(INCLUDE) $(DEFINE) $(SRCS) > $(DEPFILE)

include $(DEPFILE)


