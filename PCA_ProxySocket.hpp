#ifndef __PCA_ProxySocket___
#define __PCA_ProxySocket___

#include "PCA_GenLib.hpp"
#include "PCA_XMLParser.hpp"
#include "PCA_ServerSocket.hpp"
#include "PCA_ClientSocket.hpp"
#include "PCA_DiameterParser.hpp"


void* ResponseThread(void *thread_params);


typedef struct
{
	int client_connection;
    	string XMLCFG;
    	char client_info[20];
} ThreadParameters;




class ProxyAcceptor:public Acceptor
{
	public:
    		ProxyAcceptor(){};
    		~ProxyAcceptor();
    		ProxyAcceptor(string XMLCFG);
    		int dispatcher(int TimeOut);
    		
    		void Close();   
	public:  
   		int port;
		char host[20];
		int listenfd;
		
		
		ThreadParameters *thread_params ;
		
		
		
		string XMLCFG;
		
};


class ResponseHandler:public Acceptor
{
	public:
    		ResponseHandler(){};
    		~ResponseHandler();
    		ResponseHandler(string XMLCFG);
    		void handle_event(Connector *ClientConnector,int AcceptorConnection,char *client_info);
	public:
   		int source_id;
};



#endif



