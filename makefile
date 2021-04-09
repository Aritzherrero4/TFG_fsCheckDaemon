CC = g++
CFLAGS = -std=c++17 -lstdc++fs -lcryptopp -Wall -lsystemd 
BLFLAGS = -shared -O3
IDIR = ./include/
SDIR = ./src/
ODIR = ./obj/
BDIR = ./bin/
LDIR = ./lib/
BLAKE = ./external/blake3/
LD = -Wl,-rpath,$(LDIR) -L$(LDIR) -I$(BLAKE)

main:$(SDIR)fsCheckDaemon.cpp $(IDIR)fsCheckDaemon.hpp $(ODIR)mnode.o $(ODIR)mtree.o $(IDIR)mtree.hpp 
	g++ $(LD) -o fsCheckDaemon $(SDIR)fsCheckDaemon.cpp $(ODIR)mtree.o $(ODIR)mnode.o -lblake3 $(CFLAGS) 

$(ODIR)mnode.o: $(SDIR)mnode.cpp $(IDIR)mnode.hpp
	g++ $(LD) -c $(SDIR)mnode.cpp -o $(ODIR)mnode.o -lblake3 $(CFLAGS) 

$(ODIR)mtree.o: $(SDIR)mtree.cpp $(IDIR)mtree.hpp $(IDIR)mnode.hpp
	g++ -c $(SDIR)mtree.cpp -o $(ODIR)mtree.o $(CFLAGS)

blake: 
	gcc $(BLFLAGS) -o $(LDIR)libblake3.so $(BLAKE)blake3.c $(BLAKE)blake3_dispatch.c $(BLAKE)blake3_portable.c $(BLAKE)blake3_sse2_x86-64_unix.S $(BLAKE)blake3_sse41_x86-64_unix.S $(BLAKE)blake3_avx2_x86-64_unix.S $(BLAKE)blake3_avx512_x86-64_unix.S -lm

.PHONY : clean, all, clean-all

all: blake main
clean :
	rm $(ODIR)*
	rm fsCheckDaemon
clean-all:
	rm $(ODIR)*
	rm fsCheckDaemon
	rm $(LDIR)*

