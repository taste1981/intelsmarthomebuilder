CC=gcc
CXX=g++

CFLAGS=-I ../include -I ../../../include -I ../../../utils -I ../../../3rd-party/ezxml 

libhb_rule_engine.so:\
		hb_rule_engine.o \
		chb_rule_engine.o \
		chb_rule_xml_parser.o \
		chb_action_handler_proxy.o
	
	$(CXX) $(LDFLAGS) -shared -o ../../../bin/libhb_rule_engine.so \
		hb_rule_engine.o \
		chb_rule_engine.o \
		chb_rule_xml_parser.o \
		chb_action_handler_proxy.o

hb_rule_engine.o:hb_rule_engine.cpp
	$(CXX) $(CFLAGS) -c hb_rule_engine.cpp -fPIC
	
chb_rule_engine.o:chb_rule_engine.cpp
	$(CXX) $(CFLAGS) -c chb_rule_engine.cpp -fPIC

chb_rule_xml_parser.o:chb_rule_xml_parser.cpp
	$(CXX) $(CFLAGS) -c chb_rule_xml_parser.cpp -fPIC

chb_action_handler_proxy.o:chb_action_handler_proxy.cpp
	$(CXX) $(CFLAGS) -c chb_action_handler_proxy.cpp -fPIC
	
clean:
	rm *.o *.so

