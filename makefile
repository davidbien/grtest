# gr_test.exe makefile for Linux clang compilation.

#CXX := g++
#CXXFLAGS = -ftrack-macro-expansion -fexceptions -fuse-cxa-atexit -pthread -D_REENTRANT -D__DEBUG_THROW_VERBOSE -I /home/dbien/dv/STLport-5.2.1/stlport -I /home/dbien/dv/bienutil -I /home/dbien/dv -I /usr/include/c++/7.3.0 -g 

#CXX := c++
#CXXFLAGS = -frtti -fexceptions -fcxx-exceptions -fuse-cxa-atexit -pthread -D_REENTRANT -D__DEBUG_THROW_VERBOSE -fdelayed-template-parsing -I /home/dbien/dv/STLport-5.2.1/stlport -I /home/dbien/dv/bienutil -I /home/dbien/dv -I /usr/include/c++/7.3.0 -g 
#-D__NDEBUG_THROW

CXX := /usr/local/llvm70/bin/clang++
CXXFLAGS = -frtti -fexceptions -fcxx-exceptions -fuse-cxa-atexit -pthread -D_REENTRANT -D__DEBUG_THROW_VERBOSE -fdelayed-template-parsing -I /home/dbien/dv/STLport-5.2.1/stlport -I /home/dbien/dv/bienutil -I /home/dbien/dv -I /usr/include/c++/7.3.0 -g 
#-D__NDEBUG_THROW

SRCS = _gr_test.cpp dbgthrw.cpp

gr_test.exe: $(SRCS:.cpp=.o)
	$(CXX) -L /home/dbien/dv/STLport-5.2.1/build/lib/obj/gcc/so -lstlport -pthread -g -o gr_test.exe $(SRCS:.cpp=.o)

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
