
CXXFLAGS=-g -Wall

pathplode: pathplode.o pathp_list.o
	g++ pathplode.o pathp_list.o  -o pathplode


pathp_list.o: pathp_list.h

clean:
	rm -f *.o pathplode
