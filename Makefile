
CXX	 	= g++
CXX_FLAGS	= -Wall -g -O2 -D_REENTRANT
LDOPTS		= -pthread
MY_OBJS	= Proxy.o PCA_XMLParser.o PCA_GenLib.o PCA_DiameterParser.o PCA_ClientSocket.o PCA_ServerSocket.o PCA_ProxySocket.o 


all: Proxy

	

Proxy: $(MY_OBJS)
	$(CXX) $(LDOPTS) -o $@ $(MY_OBJS)  -static
	
clean:
	rm -f *.o Proxy
	rm -f .depend

depend: .depend
.depend:
	$(CXX) $(CXX_FLAGS) -MM *.cpp > $@

%.o:%.cpp
	$(CXX) $(CXX_FLAGS) -c -o $@ $<

-include .depend
