
DEPFILE = Makefile.dep
INCLUDE_LOCAL =  -I ../

.SUFFIXES: .cpp .o

.cpp.o :
	$(CXX) $(CXXFLAGS) $(INCLUDE_LOCAL) $(INCLUDE) $(DEFINE) -c -o $@ $<


SRCS = Header.cpp

OBJS = $(SRCS:.cpp=.o)


all: $(OBJS)

clean:
	$(RM) *.o *~ core

depend:
	gcc -MM $(INCLUDE_LOCAL) $(INCLUDE) $(DEFINE) $(SRCS) > $(DEPFILE)


include $(DEPFILE)