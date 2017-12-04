CC=gcc
CXX=g++

CFLAGS=-I ../include -I ../include/lib 

test:test.o
	$(CXX) $(LDFLAGS) -o ../bin/test test.o -L../bin/ -lhomebuilder

test.o:test.cpp
	$(CXX) $(CFLAGS) -c test.cpp

clean:
	rm *.o test 
