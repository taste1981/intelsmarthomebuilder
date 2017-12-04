CFLAGS  +=-I ../include -I ../include/lib -I../utils -I../3rd-party/ezxml
CXX=g++
CC=gcc

libhomebuilder:\
		chb_device_proxy.o \
		chb_analytic_proxy.o \
		chb_data_bus.o \
		chb_devana_man.o \
		chb_rule_engine_proxy.o \
		chb_msg_bus.o \
		chb_queue.o \
		hb.o

	$(CXX) $(LDFLAGS) -shared -o ../bin/libhomebuilder.so \
		chb_device_proxy.o \
		chb_analytic_proxy.o \
		chb_data_bus.o \
		chb_devana_man.o \
		chb_rule_engine_proxy.o \
		chb_msg_bus.o \
		chb_queue.o \
		hb.o \
		-ldl -lpthread ../3rd-party/ezxml/libezxml.a

chb_analytic_proxy.o:chb_analytic_proxy.cpp
	$(CXX) $(CFLAGS) -c chb_analytic_proxy.cpp -fPIC

chb_device_proxy.o:chb_device_proxy.cpp
	$(CXX) $(CFLAGS) -c chb_device_proxy.cpp -fPIC
	
chb_data_buf.o:chb_data_buf.cpp
	$(CXX) $(CFLAGS) -c chb_data_buf.cpp -fPIC

chb_data_bus.o:chb_data_bus.cpp
	$(CXX) $(CFLAGS) -c chb_data_bus.cpp -fPIC

chb_devana_man.o:chb_devana_man.cpp
	$(CXX) $(CFLAGS) -c chb_devana_man.cpp -fPIC
 
chb_rule_engine_proxy.o:chb_rule_engine_proxy.cpp
	$(CXX) $(CFLAGS) -c chb_rule_engine_proxy.cpp -fPIC

chb_msg_bus.o:chb_msg_bus.cpp
	$(CXX) $(CFLAGS) -c chb_msg_bus.cpp -fPIC

chb_queue.o:chb_queue.cpp
	$(CXX) $(CFLAGS) -c chb_queue.cpp -fPIC

hb.o:hb.cpp
	$(CXX) $(CFLAGS) -c hb.cpp -fPIC

clean:
	rm *.o *.so

