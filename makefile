# gr_test.exe makefile for Linux clang compilation.

# First set CXXPROJ_DEFINES to specialized defines for this specific project.
#CXXPROJ_DEFINES = -D__NDEBUG_THROW
# -D__DEBUG_THROW_VERBOSE 

# We will use the same compiler for C and CPP files - choose the C and we will choose the CPP and options.
#CC := gcc
CC := clang

include ../bienutil/makebase.inc

SRCS = _gr_test.cpp dbgthrw.cpp

gr_test.exe: $(SRCS:.cpp=.o)
	$(CXX) $(LINKFLAGS) -o gr_test.exe $(SRCS:.cpp=.o)

include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRCS))))
