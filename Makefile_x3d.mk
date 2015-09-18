#---------------------------------------------------------------
# User setting
#---------------------------------------------------------------

FILE_NAME           = Makefile_x3d.mk
SRC_DIR             = ./srcs
X3D_DIR             = ./srcs/sigverse/x3d/parser/cpp/X3DParser
AR                  = ar -cr

OUTDIR   = ./release
TARGET   = $(OUTDIR)/libsigverse-x3d.so
INCLUDES = 
LDFLAGS  = -shared 
NOMAKEDIR= .git% ./srcs/sigverse/x3d/parser/cpp/X3DParserTest%
OBJDIR   = $(OUTDIR)/x3d

GCC = g++ -fPIC
CFLAGS = -MMD -MP -std=c++11 -D__cplusplus=201103L -DdDOUBLE

#---------------------------------------------------------------
# Don't change the following
#---------------------------------------------------------------

CPPS = $(shell find $(X3D_DIR)/* -name *.cpp )
SRCS = $(filter-out $(NOMAKEDIR), $(CPPS))
DIRS = $(dir $(SRCS))
BINDIRS = $(addprefix $(OBJDIR)/, $(DIRS))

OBJS = $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(SRCS)))
DEPS = $(OBJS:.o=.d)
TILS = $(patsubst %.cpp, %.cpp~, $(SRCS))

ifeq "$(strip $(OBJDIR))" ""
	OBJDIR = .
endif

ifeq "$(strip $(DIRS))" ""
	OBJDIR = .
endif

default:
	@[ -d  $(OBJDIR)   ] || mkdir -p $(OBJDIR)
	@[ -d "$(BINDIRS)" ] || mkdir -p $(BINDIRS)
	@make -f $(FILE_NAME) all --no-print-directory

all : $(TARGET)

#print :
#	echo $(patsubst %.cpp, %.o, $(SRCS))
	
$(TARGET): $(OBJS) 
	$(GCC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: %.cpp
	$(GCC) $(CFLAGS) $(INCLUDES) -o $@ -c $<

clean:
	@rm -rf $(TARGET) $(TILS) $(OBJDIR)

-include $(DEPS)

