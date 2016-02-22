#ifndef __PCA_ClientSocket___
#define __PCA_ClientSocket___

#include "PCA_GenLib.hpp"
#include "PCA_XMLParser.hpp"


void* RequestThread(void *thread_params);

class Connector
{
	
	public:
    		Connector(){};
    		~Connector();
    		Connector(string XMLCFG);
    		int Connect();
    		int sendDataToSocket(char *Message,int Length ,int TimeOut,int WriteAttempts);
 		int readDataFromSocket(int Length,int TimeOut,int ReadAttempts,char* Message)
;
    		void handle_timeout();
    		void Close();
   
	public:
  
   		int port;
   		int bind_port;
		char host[20];
		int SocketDescriptor;
		int IsConnected;
};

#endif



