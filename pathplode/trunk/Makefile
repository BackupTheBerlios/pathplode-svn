
# $Id$

#CXXFLAGS=-O3 -fomit-frame-pointer -Wall
CXXFLAGS=-g -Wall

pathplode: pathplode.o pathp_list.o
	g++ pathplode.o pathp_list.o  -o pathplode


pathp_list.o: pathp_list.h

TAGS: *.cc *.h
	etags -l c++ *.cc *.h

clean:
	rm -f *.o pathplode core

distclean: clean
	rm -f TAGS
