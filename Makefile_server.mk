#---------------------------------------------------------------
# User setting
#---------------------------------------------------------------

FILE_NAME           = Makefile_server.mk
SRC_DIR             = ./srcs
COMMONLIB_DIR       = ./srcs/sigverse/commonlib
X3D_DIR             = ./srcs/sigverse/x3d/parser/cpp/X3DParser
SERVER_DIR          = ./srcs/sigverse/simserver
JAVA_LIB            = /usr/lib/jvm/java-6-openjdk-amd64/jre/lib/amd64/server


OUTDIR   = ./release
TARGET   = $(OUTDIR)/sigserver
INCLUDES = -I$(SRC_DIR)
#LDFLAGS  = -lm -ldl -lode -lpthread -lxerces-c -L$(OUTDIR) -lsigverse-x3d -L$(JAVA_LIB) -ljvm 
#LDFLAGS  = -L$(OUTDIR) -lsigverse-commonlib -lsigverse-x3d -L$(JAVA_LIB) -ljvm -lm -ldl -lode -lpthread -lxerces-c 
LDFLAGS  = -L$(JAVA_LIB) -ljvm -lm -ldl -lode -lpthread -lxerces-c 
NOMAKEDIR= .git% 
OBJDIR   = $(OUTDIR)/simserver

GCC = g++
CFLAGS = -g -Wall -DdDOUBLE -DSTRICT_INTERFACE -DX3DPARSER_DISABLE_DEBUG_LOG -DUSE_ODE -DUSE_XERCES -std=c++11 -D__cplusplus=201103L -MMD -MP

#---------------------------------------------------------------
# Don't change the following
#---------------------------------------------------------------

CPPS = $(shell find $(COMMONLIB_DIR)/* $(SERVER_DIR)/* $(X3D_DIR)/* -name *.cpp )
#CPPS = $(shell find $(SERVER_DIR)/* -name *.cpp )
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
	$(GCC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: %.cpp
	$(GCC) $(CFLAGS) $(INCLUDES) -o $@ -c $<

clean:
	@rm -rf $(TARGET) $(TILS) $(OBJDIR)

-include $(DEPS)

