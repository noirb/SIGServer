#---------------------------------------------------------------
# User setting
#---------------------------------------------------------------

FILE_NAME           = Makefile_server.mk
SRC_DIR             = ./srcs
SERVER_DIR          = ./srcs/sigverse/simserver
JAVA_LIB            = /usr/lib/jvm/java-6-openjdk-amd64/jre/lib/amd64/server


OUTDIR   = ./release
TARGET   = $(OUTDIR)/sigserver
INCLUDES = -I$(SRC_DIR)
LDFLAGS  = -L$(OUTDIR) -Wl,-rpath,$(SIGVERSE_PATH)/bin -lsigverse-x3d -lsigverse-commonlib -lm -ldl -lode -lpthread -lxerces-c -L$(JAVA_LIB) -ljvm 
NOMAKEDIR= .git% 
OBJDIR   = $(OUTDIR)/simserver

GCC = g++
CFLAGS = -MMD -MP -std=c++11 -D__cplusplus=201103L -DdDOUBLE -DUSE_ODE -DUSE_XERCES -DSTRICT_INTERFACE

#---------------------------------------------------------------
# Don't change the following
#---------------------------------------------------------------

CPPS = $(shell find $(SERVER_DIR)/* -name *.cpp )
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

