#ifndef __PCA_ServerSocket___
#define __PCA_ServerSocket___

#include "PCA_GenLib.hpp"
#include "PCA_XMLParser.hpp"


void* RequestThread(void *thread_params);

class Acceptor
{
	
	public:
    		Acceptor(){};
    		~Acceptor();
    		Acceptor(string XMLCFG);
    		int dispatcher(int TimeOut);
    		int sendDataToSocket(int SocketEventFD,char *Message,int Length ,int TimeOut,int WriteAttempts);
 		int readDataFromSocket(int SocketEventFD,int Length,int TimeOut,int ReadAttempts,char* Message)
;
    		void handle_timeout();
    		void Close();
   
	public:
  
   		int port;
		char host[20];
		int listenfd;
		int ConnectionPool[FD_SETSIZE];
		
};

#endif



