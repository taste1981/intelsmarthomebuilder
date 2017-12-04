CC=gcc
CXX=g++

CFLAGS=-I ../../include -I ../../utils 

libhb_dummy_analytic.so:\
		cdummy_videoana.o \
		cdummy_audioana.o \
		dummy_analytic.o
	
	$(CXX) $(LDFLAGS) -shared -o ../../bin/libhb_dummy_analytic.so \
		cdummy_videoana.o \
		cdummy_audioana.o \
		dummy_analytic.o

cdummy_videoana.o:cdummy_videoana.cpp
	$(CXX) $(CFLAGS) -c cdummy_videoana.cpp -fPIC

cdummy_audioana.o:cdummy_audioana.cpp
	$(CXX) $(CFLAGS) -c cdummy_audioana.cpp -fPIC

dummy_analytic.o:dummy_analytic.cpp
	$(CXX) $(CFLAGS) -c dummy_analytic.cpp -fPIC

clean:
	rm *.o *.so

