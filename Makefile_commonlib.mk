#---------------------------------------------------------------
# User setting
#---------------------------------------------------------------

FILE_NAME           = Makefile_commonlib.mk
SRC_DIR             = ./srcs
COMMONLIB_DIR       = ./srcs/sigverse/commonlib
AR                  = ar -cr

OUTDIR   = ./release
TARGET   = $(OUTDIR)/libsigverse-commonlib.a
INCLUDES = -I$(SRC_DIR)
LDFLAGS  = 
NOMAKEDIR= .git%
OBJDIR   = $(OUTDIR)/commonlib

GCC = g++
CFLAGS = -MMD -MP -std=c++11 -D__cplusplus=201103L -DdDOUBLE

#---------------------------------------------------------------
# Don't change the following
#---------------------------------------------------------------

CPPS = $(shell find $(COMMONLIB_DIR)/* -name *.cpp )
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
	
lib : $(OBJS) 
	$(AR) $(ARCHIVE) $(OBJS)
	
$(TARGET): 
	$(MAKE) -f $(FILE_NAME) lib "ARCHIVE=$(TARGET)"

$(OBJDIR)/%.o: %.cpp
	$(GCC) $(CFLAGS) $(INCLUDES) -o $@ -c $<

clean:
	@rm -rf $(TARGET) $(TILS) $(OBJDIR)

-include $(DEPS)

