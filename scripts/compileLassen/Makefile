# C++ compiler
CXX = clang++-3.5
CXXFLAGS = -O2 -g -emit-llvm#-DHAVE_SILO=${HAVE_SILO}
MPICXX = mpic++
# Turn this on to generate silo output files 
#  example location for silo
#HAVE_SILO = 0
#SILO_DIR = /usr/gapps/silo/current/chaos_5_x86_64_ib
#SILO_INC = -I$(SILO_DIR)/include
#SILO_LIB = -L$(SILO_DIR)/lib
#INCDIR = $(SILO_INC)

all: lassen_mpibbcount
#LIBLASSEN_SOURCES = Simulation.cxx Lassen.cxx Source.cxx \
#	           Output.cxx LassenUtil.cxx Input.cxx
#LIBLASSEN_OBJECTS = $(LIBLASSEN_SOURCES:.cxx=.o)
#LIBLASSENMPI_SOURCES = SimulationParallel.cxx SimulationMPI.cxx 
#LIBLASSENMPI_OBJECTS = $(LIBLASSENMPI_SOURCES:.cxx=.o)

SOURCES = Simulation.cxx Lassen.cxx Source.cxx \
			Output.cxx LassenUtil.cxx Input.cxx \
			lassenMPI.cxx SimulationMPI.cxx
BC_OBJECTS = $(SOURCES:.cxx=.bc)
#AR = ar csr 
.SUFFIXES: .cxx .o .bc
.cxx.bc:
	$(CXX) -c $(CXXFLAGS) -o $@ $<
# common
#libfront.a: $(LIBLASSEN_OBJECTS)
#	$(AR) libfront.a $(LIBLASSEN_OBJECTS) 
# MPI
lassenmpi.bbcounto: lassenmpi.bbcountbc
	llc-3.5 -filetype=obj lassenmpi.bbcountbc -o lassenmpi.bbcounto
lassenmpi.bbcountbc: lassenmpi.bc
	opt-3.5 -load ./libMemoryModel.so -BBCount lassenmpi.bc -o lassenmpi.bbcountbc.temp
	llvm-link-3.5 lassenmpi.bbcountbc.temp ./setup/getbbtime.bc -o lassenmpi.bbcountbc
	rm lassenmpi.bbcountbc.temp
lassenmpi.bc: $(BC_OBJECTS)
	llvm-link-3.5 $(BC_OBJECTS) -o lassenmpi.bc
lassenmpi.o: lassenmpi.bc
	llc-3.5 -filetype=obj lassenmpi.bc -o lassenmpi.o
#lassenMPI.o: lassenMPI.cxx 
#	$(MPICXX) -c $(CXXFLAGS) $(INCDIR) -o $@ $<
#SimulationMPI.o: SimulationMPI.cxx
#	$(MPICXX) -c $(CXXFLAGS) $(INCDIR) -o $@ $<
#libfrontmpi.a: $(LIBLASSENMPI_OBJECTS)
#	$(AR) libfrontmpi.a $(LIBLASSENMPI_OBJECTS) 
lassen_mpi: lassenmpi.o
	$(MPICXX) lassenmpi.o -L. -lfrontmpi -lfront $(SILO_LIB) -o lassen_mpi
lassen_mpibbcount: lassenmpi.bbcounto
	$(MPICXX) lassenmpi.bbcounto -L. -lfrontmpi -lfront $(SILO_LIB) -o lassen_mpibbcount
clean:
	rm -f *.o lib*.a *.decl.h *.def.h *.bc *.bbcount* lassen_serial lassen_charm lassen_mpi lassen_charmmpi
veryclean: clean
	rm -f *.silo *.core *~ *.btr *.TVD.v3breakpoints charmrun
