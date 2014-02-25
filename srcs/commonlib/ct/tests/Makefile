
CXX = g++
LD = g++

INCLUDE = -I../../ -I../

#DEFINE = -DUSE_ODE
DEBUG = -g

CFLAGS = $(INCLUDE) $(DEFINE) $(DEBUG)

.SUFFIXES : .cpp .o
.cpp.o:
	$(CXX) $(CFLAGS) -c -o $@ $< 

LDFLAGS = -L ../../ -lirwas-common -lode

SRCS = test1.cpp 

OBJS = $(SRCS:.cpp=.o)

TARGET = test1

$(TARGET) : $(OBJS) 
	$(LD) -o $(TARGET) $(TARGET).o $(LDFLAGS)

LIB_OBJS = $(LIB_SRCS:.cpp=.o)

UNIT_TARGET = test1

unit: $(LIB_OBJS)
	$(MAKE) compile "TEST_OBJ=$(UNIT_TARGET).o"
	./$(UNIT_TARGET)


compile: $(TEST_OBJ)
	$(LD) -o $(TEST_OBJ:.o=) $(TEST_OBJ) $(LIB_OBJS) -lcppunit

clean:
	$(RM) *.o
