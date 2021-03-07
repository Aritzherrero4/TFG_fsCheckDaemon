CC = g++
CFLAGS = -std=c++17 -lstdc++fs -lcryptopp -Wall -lsystemd
IDIR = ./include/
SDIR = ./src/
ODIR = ./obj/
BDIR = ./bin/

main: $(SDIR)fsCheckDaemon.cpp $(IDIR)fsCheckDaemon.hpp $(ODIR)mnode.o $(ODIR)mtree.o $(IDIR)mtree.hpp 
	g++ -o fsCheckDaemon $(SDIR)fsCheckDaemon.cpp $(ODIR)mtree.o $(ODIR)mnode.o $(CFLAGS)

$(ODIR)mnode.o: $(SDIR)mnode.cpp $(IDIR)mnode.hpp
	g++ -c $(SDIR)mnode.cpp -o $(ODIR)mnode.o $(CFLAGS)

$(ODIR)mtree.o: $(SDIR)mtree.cpp $(IDIR)mtree.hpp $(IDIR)mnode.hpp
	g++ -c $(SDIR)mtree.cpp -o $(ODIR)mtree.o $(CFLAGS) 

.PHONY : clean
clean :
	rm $(ODIR)*
	rm main

