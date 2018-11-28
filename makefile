

gr_test.exe: _gr_test.o dbgthrw.o
	g++ -g -o gr_test.exe _gr_test.o dbgthrw.o

_gr_test.o: _gr_test.cpp _graph.h _gr_inc.h _gr_trt.h _gr_def.h _gr_safe.h _gr_type.h _gr_dtor.h _gr_copy.h _gr_iter.h _gr_sitr.h _gr_pitr.h _gr_titr.h _gr_gitr.h _gr_sril.h _gr_dump.h _gr_outp.h _gr_inpt.h _gr_inpw.h _gr_stin.h makefile
	g++ -I /bienlibs -I /usr/include/g++ -ftemplate-depth-30 -D__NDEBUG_THROW -g -c -o _gr_test.o _gr_test.cpp

dbgthrw.o: ../bienutil/dbgthrw.cpp ../bienutil/_dbgthrw.h makefile
	g++ -I /bienlibs -I /usr/include/g++ -ftemplate-depth-30 -D__NDEBUG_THROW -g -c -o dbgthrw.o dbgthrw.cpp
