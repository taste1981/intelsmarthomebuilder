CC=gcc
CXX=g++

CFLAGS=-I ../../include -I ../../utils 

libhb_dummy_device.so:\
		cdummy_device.o \
		dummy_device.o
	
	$(CXX) $(LDFLAGS) -shared -o ../../bin/libhb_dummy_device.so \
		cdummy_device.o \
		dummy_device.o

cdummy_device.o:cdummy_device.cpp
	$(CXX) $(CFLAGS) -c cdummy_device.cpp -fPIC

dummy_device.o:dummy_device.cpp
	$(CXX) $(CFLAGS) -c dummy_device.cpp -fPIC

clean:
	rm *.o *.so
