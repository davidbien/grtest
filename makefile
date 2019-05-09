# gr_test.exe makefile for Linux clang compilation.

# First set MOD_DEFINES to specialized defines for this specific project.
#MOD_DEFINES = -D__NDEBUG_THROW
# -D__DEBUG_THROW_VERBOSE 

# We will use the same compiler for C and CPP files - choose the C and we will choose the CPP and options.
#CC := gcc
CC := clang
#CC := /usr/local/cuda/bin/nvcc

MAKEBASE = ../bienutil/makebase.mk
include $(MAKEBASE)

SRCS = _gr_test.cpp dbgthrw.cpp

$(BUILD_DIR)/gr_test.exe: $(patsubst %,$(BUILD_DIR)/%.o,$(basename $(SRCS)))
	$(CXX) $(LINKFLAGS) -o $(BUILD_DIR)/gr_test.exe $(patsubst %,$(BUILD_DIR)/%.o,$(basename $(SRCS)))

include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRCS))))
