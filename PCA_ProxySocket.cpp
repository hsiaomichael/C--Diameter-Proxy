

#include "PCA_ProxySocket.hpp"


//static pthread_mutex_t thread_parameter_mutex;
			
			


/**********************************************************************************


**********************************************************************************/   
ResponseHandler::ResponseHandler(string XMLCFG)
{
	
	try
	{
		Debug(1, ("ResponseHandler init"));
		
		
	}
	catch (...)
	{
  		Debug(0, ("ResponseHandler constructor Error"));
  		throw "ResponseHandler constructor Error";
	}
   
}

/**********************************************************************************


**********************************************************************************/   
void ResponseHandler::handle_event(Connector *ClientConnector,int AcceptorConnection,char *client_info )
{
	char Message[2048],Message_Length[5],Message_Header[63],Message_Body[1024];
   
	
	
	//int result;
	//int Length;
	int response_status = 0;
	int Timeout = 1;
	int diameter_length = 0;
	int response_ret = 0;
					  				
		  		
	
	char *pointer_to_socket_message;
	
	
	DiameterParser *parser  = new DiameterParser();
      
		  	
	        					
	try
	{
		Debug(1, ("ResponseHandler handle_event init"));		
		
		while (GetTerminateFlag () != TRUE)
		{
			/***************************************************************************************************
			
			 Read Request from Client
			
			***************************************************************************************************/
			
	 		memset(Message,0x00,2048);
	 		memset(Message_Length,0x00,5);
	 		memset(Message_Header,0x00,63);
	 		memset(Message_Body,0x00,1024);
	 		
	 		
	 		response_status = 0;
	 		
	 		
			
	 		response_ret = readDataFromSocket(AcceptorConnection,4,Timeout,1,Message_Header);

	 		if (response_ret == 4)
			{
				
				//hex_dumper(1,"recv header from cient:",Message_Header,response_ret);
				pointer_to_socket_message = Message_Header;
				diameter_length = 0;
				
		  		memcpy( &Message_Length[1],pointer_to_socket_message+1,3);
  				memcpy(&diameter_length,Message_Length,4);  						
  				
  				diameter_length = ntohl(diameter_length);
  				diameter_length = diameter_length - 4 ;
	 			if (diameter_length > 0)  						
  				{
  					Debug(2, ("read length from client value=<%d>,id=<%d>",diameter_length,AcceptorConnection));	
					
	 				if (diameter_length > 1000) 
  					{
  						Debug(0, ("ResponseHandler readDataFromSocket diameter_length > 1000 error (Server) error id=<%d>",AcceptorConnection));	
						break;   
  					} 	
	 				
	 				pointer_to_socket_message = Message;  						
  					memcpy(pointer_to_socket_message,Message_Header,4);  
  					
  					
  					response_ret = readDataFromSocket(AcceptorConnection,diameter_length,Timeout,1,Message_Body);
  					
  					if (response_ret == diameter_length )
  					{ 	
  						//hex_dumper(1,"recv Message_Body",Message_Body,response_ret);
   		 				memcpy(pointer_to_socket_message+4,Message_Body,diameter_length);
   		 			}
   		 			else
   		 			{	
   		 				Debug(0, ("ResponseHandler readDataFromSocket Message_Body socket length error we want <%d> , but got <%d> error id=<%d>",diameter_length-4,response_ret,AcceptorConnection));	
						break;
   		 			}
   		 			
   		 			// Send to Server 
   		 			
   		 			hex_dumper(3,"recv full diameter Message",Message,diameter_length + 4);
   		 			Debug(2, ("recv from Client id=<%d>",AcceptorConnection));
   		 			
               				parser->parse("Client",client_info,Message,diameter_length + 4);
               						
   		 			
   		 			response_ret = ClientConnector->sendDataToSocket(Message,diameter_length+4,1,1);
   		 			if ( response_ret == (diameter_length+4) )
   		 			{ 
   		 				Debug(2, ("send to Server ok id=<%d>",AcceptorConnection));	
   		 			}
   		 			else
   		 			{	
   		 				Debug(0, ("send to Server error response_ret=<%d>, id=<%d>",response_ret,AcceptorConnection));
   		 				break;	
   		 			
   		 			}
			
  				}							  						
  					
  				else
  				{
  					
	 				Debug(0, ( "Diameter body Length <= 0  break  id=<%d>",AcceptorConnection));
	 				break;
  				}
	 		
	 		}
	 		else
	 		{
	 			if (response_ret == -1)
	 			{
	 				Debug(2, ( "Read Request from Client Timeout id=<%d>",AcceptorConnection));
	 				
	 			}
	 			else
	 			{
	 				if (response_ret == 0)
	 				{
	 					Debug(0, ( "client close connection  id=<%d>",AcceptorConnection));
	 				}
	 				else
	 				{
	 					Debug(0, ( "Read Diameter Length %d < 4  break  id=<%d>",response_ret,AcceptorConnection));
	 				}
	 				break;
	 			}	
				
	 		}
	 	
	 	
	 	
	 		/***************************************************************************************************
			
			 Read Request from Server
			
			***************************************************************************************************/
			
			memset(Message,0x00,2048);
	 		memset(Message_Length,0x00,5);
	 		memset(Message_Header,0x00,63);
	 		memset(Message_Body,0x00,1024);
	 		
	 		
	 		response_status = 0;
			
			
			response_ret = ClientConnector->readDataFromSocket(4,Timeout,1,Message_Header);
	 		
	 		if (response_ret == 4)
			{
				
				//hex_dumper(1,"recv header from cient:",Message_Header,response_ret);
				pointer_to_socket_message = Message_Header;
				diameter_length = 0;
				
		  		memcpy( &Message_Length[1],pointer_to_socket_message+1,3);
  				memcpy(&diameter_length,Message_Length,4);  						
  				
  				diameter_length = ntohl(diameter_length);
  				diameter_length = diameter_length - 4 ;
	 			if (diameter_length > 0)  						
  				{
  					Debug(2, ("read length from client value=<%d>,id=<%d>",diameter_length,AcceptorConnection));	
					
	 				if (diameter_length > 1000) 
  					{
  						Debug(0, ("ResponseHandler read diameter length from Server diameter_length > 1000 error (Server) error id=<%d>",AcceptorConnection));	
						break;   
  					} 	
	 				
	 				pointer_to_socket_message = Message;  						
  					memcpy(pointer_to_socket_message,Message_Header,4);  
  					
  					response_ret = ClientConnector->readDataFromSocket(diameter_length,Timeout,1,Message_Body);
	 		
  					
  					if (response_ret == diameter_length )
  					{ 	
  						
  						memcpy(pointer_to_socket_message+4,Message_Body,diameter_length);
   		 			}
   		 			else
   		 			{	
   		 				Debug(0, ("ResponseHandler readDataFromSocket Message_Body socket length error we want <%d> , but got <%d> error id=<%d>",diameter_length-4,response_ret,AcceptorConnection));	
						break;
   		 			}
   		 			
   		 			Debug(2, ("recv from Server id=<%d>",AcceptorConnection));	
   		 			parser->parse("Server",client_info,Message,diameter_length + 4);
               				
   		 			hex_dumper(3,"recv full diameter Message",Message,diameter_length + 4);
   		 			
   		 			
   		 			response_ret = sendDataToSocket(AcceptorConnection,Message,diameter_length+4,5,1);
         				
   		 			if ( response_ret == (diameter_length+4) )
   		 			{ 
   		 				Debug(2, ("send back to Client ok id=<%d>",AcceptorConnection));	
   		 				
   		 			}
   		 			else
   		 			{	
   		 				Debug(0, ("send back to Client error response_ret=<%d>,id=<%d>",response_ret,AcceptorConnection));	
   		 				break;
   		 			
   		 			}
			
  				}							  						
  					
  				else
  				{
  					
	 				Debug(0, ( "Read from Server Diameter body Length <= 0  break  id=<%d>",AcceptorConnection));
	 				break;
  				}
	 		
	 		}
	 		else
	 		{
	 			if (response_ret == -1)
	 			{
	 				Debug(2, ( "Read  from Server Timeout id=<%d>",AcceptorConnection));
	 				
	 			}
	 			else
	 			{
	 				if (response_ret == 0)
	 				{
	 					Debug(0, ( "Server close connection  id=<%d>",AcceptorConnection));
	 				}
	 				else
	 				{
	 					Debug(0, ( "Read Server Diameter Length %d < 4  break  id=<%d>",response_ret,AcceptorConnection));
	 				}
	 				break;
	 			}	
				
	 		}
	 	
			
			
			
			
		}
	
		
		
		ClientConnector->Close();
		Debug(1, ("ResponseHandler  handle_event terminate AcceptorConnection=<%d>",AcceptorConnection));
		close(AcceptorConnection);
	}
	catch (...)
	{
  		Debug(0, ("handle_event Error"));
  		throw "ResponseHandler handle_event Error";
  		close(AcceptorConnection);
  		ClientConnector->Close();
	}
   
}

/**********************************************************************************


**********************************************************************************/   
ResponseHandler::~ResponseHandler()
{    
  
  	Debug(2, ("ResponseHandler destructor Init"));
  	
  	
  	 
  	Debug(2, ("ResponseHandler destructor OK"));
}








/**********************************************************************************


**********************************************************************************/  


void * ResponseThread(void *thread_params)         					
{
  	int error;
  	
  	string XMLCFG ;
  	Connector *TCPServer;
  	int server_connector;
  	int ret=0;
  	
  	char thread_information[70];
  
  	
  	
  	
  	ThreadParameters  *thread_parameter  = (ThreadParameters *) thread_params;
   
  	Debug(2, ("ResponseThread Init"));
  	
  	
     	error = pthread_detach(pthread_self());
  	
  	if (error != 0)
  	{
     		Error(("::pthread_detach() failed with errno %d.", error));
     		SetTerminateFlag(NULL);
 	}
    
    	Debug(1, ("start of response handler"));
    
    	
    	
    	server_connector = thread_parameter->client_connection;   	
    	
    	XMLCFG = thread_parameter->XMLCFG;
    	
    	
    	//Debug(1, ("XMLCFG = <%s>",XMLCFG.c_str()));
    	
    	//TCPServer = NULL;
	
    	TCPServer = new Connector(XMLCFG);
    	
    	
    	try
    	{    	
    		ret = TCPServer->Connect();	
    		if (ret == 1)
    		{
    			
    			
    			try
			{
      				
      				sprintf(thread_information,"%s:%d",thread_parameter->client_info,server_connector);
    	
      				ResponseHandler *ServerResponseHandler = new ResponseHandler(XMLCFG);
      				ServerResponseHandler->handle_event(TCPServer,server_connector,thread_information);
      		
      				delete ServerResponseHandler;
      		
      				Debug(1, ("ResponseThread Ok ..."));
			}
    			catch (...)
    			{		
      				Debug(1, ("ResponseThread error "));		
    			}
    		}
    		else
    		{
    			Debug(1, ("Can not connect to server exit thread ..."));
    		}
	}
	catch(...)
	{
		Debug(1, ("Fail to connect to server , let response thread re-connect it "));
	}
    	
    	
    	
    	
    	Debug(2, ("normal end of ResponseThread"));
    	
    	delete thread_parameter;
    	delete TCPServer;
    	
   	return NULL;
    
} 

/******************************************************************************

******************************************************************************/


ProxyAcceptor::ProxyAcceptor(string XMLCFG)
{
	
  try
  {
  	Debug(9, ("ProxyAcceptor constructor Init"));
  	
  	//static pthread_mutex_t thread_parameter_mutex;
	//if ( pthread_mutex_init ( &thread_parameter_mutex, NULL ) )
	//{
	//	Error ( ( "pthread_mutex_init failed with %d  - shutdown process", errno ) );
	//	throw "pthread_mutex_init failed";
	//}
		
	
	this->XMLCFG = XMLCFG;
       
	
	PCA_XMLParser *XMLParser = NULL;
	string Tag;
   	string TagValue;
    	char *tail;  	

   	XMLParser = new PCA_XMLParser(XMLCFG);        
        
   	Tag = "LISTEN_HOST";
   	TagValue = XMLParser->getXMLTagValue(XMLCFG,Tag);
   	Debug(2, ("%s=%s",Tag.c_str(),TagValue.c_str()));
   	strcpy(this->host,TagValue.c_str());   	
   	
   	Tag = "LISTEN_PORT";
   	TagValue = XMLParser->getXMLTagValue(XMLCFG,Tag);
   	Debug(2, ("%s=<%s>",Tag.c_str(),TagValue.c_str()));
   	this->port = strtol(TagValue.c_str(), &tail, 10);   
   	
   	delete XMLParser;  
   		
	
	
	int RC = 0;
	this->listenfd = 0;
	
	Debug(1, ("Listen Host=<%s>,Port=<%d>",this->host,this->port));
	
	struct in_addr IPAddress;            /* Used to hold INADDR_ANY */
	IPAddress.s_addr = INADDR_ANY;
	struct sockaddr_in SocketAddress;	
	memset (&SocketAddress, 0, sizeof SocketAddress);
	memcpy (&SocketAddress.sin_addr,&IPAddress,sizeof IPAddress);
	SocketAddress.sin_port =  htons(port);
	SocketAddress.sin_family = AF_INET;
	SocketAddress.sin_addr.s_addr = inet_addr(this->host);

	
	/*-----Create the advertising socket-----*/
	Debug(2, ("Create the listen socket"));
	this->listenfd = socket (AF_INET, SOCK_STREAM, 0);
	if (this->listenfd < 0)
	{   
		Debug(0, ("Failed to create listen socket - errno = %d (%s)",errno, strerror(errno) ));
		RC = -1;
	}
	
	Debug(2, ("Set SO_REUSEADDR server socket optiont"));
	static const int  NON_ZERO = 1;	
	if (setsockopt(this->listenfd,SOL_SOCKET,SO_REUSEADDR,&NON_ZERO,sizeof NON_ZERO) < 0)
	{  
		Debug(0, ("Failed to set listen socket option SO_REUSEADDR - errno = %d (%s)",errno, strerror(errno) ));
		close(this->listenfd);
    		RC = -1;
  	}
  	
  	Debug(2, ("Set SO_KEEPALIVE server socket option")); 
	if (setsockopt(this->listenfd,SOL_SOCKET,SO_KEEPALIVE,&NON_ZERO,sizeof NON_ZERO ) < 0)
  	{  
     		Debug(0,("Failed to set listen socket option SO_KEEPALIVE on advertising socket - errno = %d (%s)",errno, strerror(errno)));
     		close(this->listenfd);
     		RC = -1;
  	}
  	
  	Debug(2, ("Set SO_LINGER listen socket option to discard data on close")); 
	struct linger LingerData;
	LingerData.l_onoff  = 0;
	LingerData.l_linger = 0;
  	if (setsockopt(this->listenfd,SOL_SOCKET,SO_LINGER,&LingerData,sizeof LingerData ) < 0)
	{     
		Debug(0,( "Failed to set listen socket option SO_LINGER on advertising socket - errno = %d (%s)",errno, strerror(errno)));
     
		close(this->listenfd);
 		RC = -1;
  	}
   	
  	Debug(2, ("bind")); 
    	
    	if (bind(this->listenfd,(struct sockaddr *) &SocketAddress,sizeof SocketAddress) < 0)
    	{      
      		Debug(0, ("bind() failed on listen socket - errno = %d (%s)",errno, strerror(errno)));
      
      		close(this->listenfd);
      		RC = -1;
    	}
    	Debug(1, ("listen max queue 10")); 
    	
    	if (listen(this->listenfd,10) < 0)
	{
     
      		Debug(0, ("listen() failed on listen socket, errno = %d (%s)",errno, strerror(errno)));
      		close(this->listenfd);
      		RC = -1;
    	}  
        /* Got here, socket set up OK */
  
	Debug(2,( "Socket listened OK, No errors so far socket fd = <%d>",this->listenfd));
  	 
	Debug(9, ("ProxyAcceptor constructor ok"));
	if (RC == -1)
	{
		
  		Debug(0, ("init socket error"));
  		throw "init socket error";
	}
  }
  
  catch (...)
  {
  	Debug(0, ("ProxyAcceptor constructor Error"));
  	throw "ProxyAcceptor constructor Error";
  }
   
}


/******************************************************************************/
/*                                                                            */
/* Server Dispatcher				                              */
/*                                                                            */
/******************************************************************************/
 
int ProxyAcceptor::dispatcher(int TimeOut)
{     
	pthread_t id;
	
	int connection;
	int res;
	socklen_t clilen;
	struct sockaddr_in cliaddr;
	fd_set ReadSet,AllSet;	
	struct timeval 	socket_TimeOut;
  	socket_TimeOut.tv_sec = TimeOut;
	socket_TimeOut.tv_usec = 0;
	int SocketConnection = 0;
	int max_fd = this->listenfd;	

	
	Debug(0, ("dispatcher server starting this->listenfd=<%d>",this->listenfd));		
	FD_ZERO(&ReadSet);
	FD_ZERO(&AllSet);
	FD_SET(this->listenfd, &AllSet);
	
	while (GetTerminateFlag () != TRUE)
	{
		
		Debug ( 2, ("server ready for new client connection" )); 		
 		//socket_TimeOut.tv_sec = TimeOut;
 		socket_TimeOut.tv_sec = 5;
		socket_TimeOut.tv_usec = 0;	
 			
		ReadSet = AllSet;
		if ( ( res = select(max_fd+1,  &ReadSet, NULL, NULL , &socket_TimeOut)) == -1 )
     		{     			
        		if( errno != EINTR )
        		{
           			Debug ( 0, ("ProxyAcceptor - error when calling select: %d", errno ));
           			break;
        		}
      		}		
		
		if (res == 0 ) 
      		{
      	 		Debug ( 2, ("ProxyAcceptor - listener dispatcher server loop %d seconds timeout ",TimeOut));
      		}
      		else if (res > 0 ) 
		{	
			
			if (FD_ISSET (this->listenfd, &ReadSet))
			{
			
				clilen = sizeof(cliaddr);
      				connection = accept(this->listenfd, (struct sockaddr *) &cliaddr, &clilen);
      				if ( connection == -1)
      				{
      					Debug(0, ("ProxyAcceptor - accept failed: %d", errno));
      					break;
      				}
      				else
      				{
      					char client_info[20];
      					strcpy(client_info,inet_ntoa(cliaddr.sin_addr));
  	  				Debug (0,("ProxyAcceptor - Dispatcher New Connection from <%s>:%d fd=<%d> ", client_info, cliaddr.sin_port,connection ));
  	  				
  	  				thread_params = new ThreadParameters();     

					if (thread_params == NULL)
					{
						Error(("malloc for thread_params failed - shutdown process"));
						throw "malloc for thread_params failed - shutdown process";
					}		
					
       					 thread_params->XMLCFG = this->XMLCFG;
       					 thread_params->client_connection = connection;
       					 sprintf(thread_params->client_info,"%s:%d",client_info,cliaddr.sin_port);
  	  				
	    	
  	  				if (pthread_create(&id, NULL, &ResponseThread,(void *)thread_params))
       					{
        					Error(("::pthread_create  for ResponseThread failed, terminating"));
        					SetTerminateFlag(NULL);
        					throw "Create Thread Error";
					}
      				}
	    		}
	    		
	    				
		}
		else
		{				
               		Debug (0,("Close client connection fd=<%d>",SocketConnection));
               		close (SocketConnection);
      		}
      		
      		    	  
  	}/*end while */

  	Debug(0, ("dispatcher OK"));  	
  	
  	return 0;
}




 

/**********************************************************************************


**********************************************************************************/   
void ProxyAcceptor::Close()
{    
  
  	Debug(2, ("ProxyAcceptor Close Init"));
  	
  	
  	if (this->listenfd)
  	{
  		Debug(0, ("ProxyAcceptor Close : close listen this->listenfd=<%d>",this->listenfd));
  		close(this->listenfd);
  		this->listenfd = 0;
  	}
  	
        
        
  	Debug(2, ("ProxyAcceptor Close OK"));
 
}

/**********************************************************************************


**********************************************************************************/   
ProxyAcceptor::~ProxyAcceptor()
{    
  
  	Debug(2, ("ProxyAcceptor destructor Init"));
  	
  	
  	
  	Debug(2, ("ProxyAcceptor destructor OK"));
 
}

