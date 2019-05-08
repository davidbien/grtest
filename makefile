# gr_test.exe makefile for Linux clang compilation.

$(info $(HOME))

CXXFLAGS_DEFINES = -D_REENTRANT -D_STLP_MEMBER_TEMPLATES -D_STLP_USE_NAMESPACES -D_STLP_CLASS_PARTIAL_SPECIALIZATION -D_STLP_FUNCTION_TMPL_PARTIAL_ORDER
# -D__NDEBUG_THROW 
# -D__DEBUG_THROW_VERBOSE 
CXXFLAGS_INCLUDES = -I $(HOME)/dv/bienutil -I $(HOME)/dv
CXXFLAGS_BASE = $(CXXFLAGS_DEFINES) $(CXXFLAGS_INCLUDES)
CXXANDLINKFLAGS_BASE = -std=gnu++17 -pthread -g
CC := cc

#CXX := g++
#CXXANDLINKFLAGS = $(CXXANDLINKFLAGS_BASE)
#CXXFLAGS = $(CXXFLAGS_BASE) $(CXXANDLINKFLAGS)

CXX := clang 
# ASAN_OPTIONS=check_initialization_order=1
# ASAN_OPTIONS=detect_leaks=1
# ASAN_OPTIONS=detect_stack_use_after_return=1
#CLANG_ADDR_SANITIZE = -fsanitize=address -fsanitize-address-use-after-scope

# MSAN_OPTIONS=poison_in_dtor=1
# ASAN_OPTIONS=detect_leaks=1
# ASAN_OPTIONS=detect_stack_use_after_return=1
#  -D__GR_TEST_NOIOSTREAMS because the IO streams are apparently offending the MEMSAN a bunch.
#CLANG_MEM_SANITIZE = -fsanitize=memory -fsanitize-memory-track-origins -fsanitize-memory-use-after-dtor -D__GR_TEST_NOIOSTREAMS
#CLANGSANITIZE = $(CLANG_ADDR_SANITIZE) $(CLANG_MEM_SANITIZE) -fsanitize-blacklist=blacklist.txt -fno-omit-frame-pointer
CXXANDLINKFLAGS = $(CLANGSANITIZE) $(CXXANDLINKFLAGS_BASE)
CXXFLAGS = $(CXXFLAGS_BASE) -fdelayed-template-parsing $(CXXANDLINKFLAGS)

#CXX := /usr/local/llvm70/bin/clang++
#CXXFLAGS = -frtti -fexceptions -fcxx-exceptions -fuse-cxa-atexit -pthread -D_REENTRANT -fdelayed-template-parsing -I $(HOME)/dv/bienutil -I $(HOME)/dv -I /usr/include/c++/7.3.0 -g
#-D__NDEBUG_THROW -D__DEBUG_THROW_VERBOSE 

SRCS = _gr_test.cpp dbgthrw.cpp

gr_test.exe: $(SRCS:.cpp=.o)
	$(CXX) $(CXXANDLINKFLAGS) -lstdc++ -o gr_test.exe $(SRCS:.cpp=.o)

DEPDIR := .d
$(shell mkdir -p $(DEPDIR) >/dev/null)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
COMPILE.cc = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
POSTCOMPILE = @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

%.o : %.c
%.o : %.c $(DEPDIR)/%.d makefile
	$(COMPILE.c) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

%.o : %.cc
%.o : %.cc $(DEPDIR)/%.d makefile
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

%.o : %.cxx
%.o : %.cxx $(DEPDIR)/%.d makefile
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

%.o : %.cpp
%.o : %.cpp $(DEPDIR)/%.d makefile
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRCS))))
