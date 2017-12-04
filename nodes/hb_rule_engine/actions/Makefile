CC=gcc
CXX=g++

CFLAGS=-I ../include -I ../../../include -I ../../../utils

libhb_action_handler.so:\
		chb_action_handler.o
	
	$(CXX) $(LDFLAGS) -shared -o ../../../bin/libhb_action_handler.so \
		chb_action_handler.o
		
chb_action_handler.o:chb_action_handler.cpp
	$(CXX) $(CFLAGS) -c chb_action_handler.cpp -fPIC

clean:
	rm *.o *.so
