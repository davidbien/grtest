

#_gr_test.o: _gr_test.cpp _graph.h _gr_inc.h _gr_trt.h _gr_def.h _gr_safe.h _gr_type.h _gr_dtor.h _gr_copy.h _gr_iter.h _gr_sitr.h _gr_pitr.h _gr_titr.h _gr_gitr.h _gr_sril.h _gr_dump.h _gr_outp.h _gr_inpt.h _gr_inpw.h _gr_stin.h makefile
#	c++ -MD -MF _gr_test.d -fdelayed-template-parsing -I /home/dbien/dv/STLport-5.2.1/stlport -I /home/dbien/dv/bienutil -I /home/dbien/dv -I /usr/include/c++/7.3.0 -D__NDEBUG_THROW -g -c -o _gr_test.o _gr_test.cpp

#dbgthrw.o: ../bienutil/dbgthrw.cpp ../bienutil/_dbgthrw.h makefile
#	c++ -MD -MF dbgthrw.d -fdelayed-template-parsing -I /home/dbien/dv/STLport-5.2.1/stlport -I /home/dbien/dv/bienutil -I /home/dbien/dv -I /usr/include/c++/7.3.0 -D__NDEBUG_THROW -g -c -o dbgthrw.o ../bienutil/dbgthrw.cpp

SRCS = _gr_test.cpp dbgthrw.cpp

gr_test.exe: $(SRCS:.cpp=.o)
	c++ -L /home/dbien/dv/STLport-5.2.1/build/lib/obj/gcc/so -lstlport -pthread -g -o gr_test.exe $(SRCS:.cpp=.o)

CXX := c++
CXXFLAGS = -fdelayed-template-parsing -I /home/dbien/dv/STLport-5.2.1/stlport -I /home/dbien/dv/bienutil -I /home/dbien/dv -I /usr/include/c++/7.3.0 -g 
#-D__NDEBUG_THROW

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
