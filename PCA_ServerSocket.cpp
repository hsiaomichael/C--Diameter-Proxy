#include "PCA_ServerSocket.hpp"

/******************************************************************************

******************************************************************************/


Acceptor::Acceptor(string XMLCFG)
{
	
  try
  {
	
	Debug(9, ("Acceptor constructor Init"));
	
	
	int RC = 0;
	this->listenfd = 0;
	
	int i ;
	for (i=0;i< FD_SETSIZE;i++)
	{
		this->ConnectionPool[i] = -1;
	}
	
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
		Debug(1, ("Failed to create listen socket - errno = %d (%s)",errno, strerror(errno) ));
		RC = -1;
	}
	
	Debug(2, ("Set SO_REUSEADDR server socket optiont"));
	static const int  NON_ZERO = 1;	
	if (setsockopt(this->listenfd,SOL_SOCKET,SO_REUSEADDR,&NON_ZERO,sizeof NON_ZERO) < 0)
	{  
		Debug(1, ("Failed to set listen socket option SO_REUSEADDR - errno = %d (%s)",errno, strerror(errno) ));
		close(this->listenfd);
    		RC = -1;
  	}
  	
  	Debug(2, ("Set SO_KEEPALIVE server socket option")); 
	if (setsockopt(this->listenfd,SOL_SOCKET,SO_KEEPALIVE,&NON_ZERO,sizeof NON_ZERO ) < 0)
  	{  
     		Debug(1,("Failed to set listen socket option SO_KEEPALIVE on advertising socket - errno = %d (%s)",errno, strerror(errno)));
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
      		Debug(2, ("bind() failed on listen socket - errno = %d (%s)",errno, strerror(errno)));
      
      		close(this->listenfd);
      		RC = -1;
    	}
    	Debug(1, ("listen max queue 10")); 
    	
    	if (listen(this->listenfd,10) < 0)
	{
     
      		Debug(2, ("listen() failed on listen socket, errno = %d (%s)",errno, strerror(errno)));
      		close(this->listenfd);
      		RC = -1;
    	}  
        /* Got here, socket set up OK */
  
	Debug(2,( "Socket listened OK, No errors so far socket fd = <%d>",this->listenfd));
  	 
	Debug(9, ("Acceptor constructor ok"));
	if (RC == -1)
	{
		
  		Debug(1, ("init socket error"));
  		throw "init socket error";
	}
  }
  catch (...)
  {
  	Debug(0, ("Acceptor constructor Error"));
  	throw "Acceptor constructor Error";
  }
   
}


/******************************************************************************/
/*                                                                            */
/* Server Dispatcher				                              */
/*                                                                            */
/******************************************************************************/
 
int Acceptor::dispatcher(int TimeOut)
{     
	int connection;
	int res;
	socklen_t clilen;
	struct sockaddr_in cliaddr;
	fd_set ReadSet,AllSet;
	 				
	struct timeval 	socket_TimeOut;
  	socket_TimeOut.tv_sec = TimeOut;
	socket_TimeOut.tv_usec = 0;	
	
	int SocketConnection = 0;
	
 	
	Debug(0, ("dispatcher server starting this->listenfd=<%d>",this->listenfd));
	
		
	FD_ZERO(&ReadSet);
	FD_ZERO(&AllSet);
	FD_SET(this->listenfd, &AllSet);
			
	int  ret,i;  
   	
   	char Message[4096];
   	int Length=4096;
	
	int max_fd,max_index_for_ConnectionPool;
	
	max_index_for_ConnectionPool = 1;
	
	max_fd = this->listenfd;
			
	
	while (GetTerminateFlag () != TRUE)
	{
		
		Debug ( 2, ("server ready for new client connection" ));
 		
 		socket_TimeOut.tv_sec = TimeOut;
		socket_TimeOut.tv_usec = 0;	
 			
		ReadSet = AllSet;
		if ( ( res = select(max_fd+1,  &ReadSet, NULL, NULL , &socket_TimeOut)) == -1 )
     		{
     			
        		if( errno != EINTR )
        		{
           			Debug ( 0, ("Acceptor - error when calling select: %d", errno ));
           			break;
        		}
      		}		
		
		if (res == 0 ) 
      		{
      			
      	 		Debug ( 2, ("Acceptor - listener dispatcher server loop %d seconds timeout ",TimeOut));
      		}
      		else if (res > 0 ) 
		{	
			
			if (FD_ISSET (this->listenfd, &ReadSet))
			{
			
				clilen = sizeof(cliaddr);
      				connection = accept(this->listenfd, (struct sockaddr *) &cliaddr, &clilen);
      				if ( connection == -1)
      				{
      					Debug(0, ("Acceptor - accept failed: %d", errno));
      					break;
      				}
      				else
      				{
      					char client_info[20];
      					strcpy(client_info,inet_ntoa(cliaddr.sin_addr));
  	  				Debug (0,("Acceptor - Dispatcher New Connection from <%s>:%d fd=<%d> ", client_info, cliaddr.sin_port,connection ));
  	  				FD_SET (connection, &AllSet);
  	  						
  	  				
  	  				for (i=0;i< FD_SETSIZE;i++)
					{
						if (this->ConnectionPool[i] < 0 )
						{
							this->ConnectionPool[i] = connection;
							break;
						}
					}
					if (i ==  FD_SETSIZE)	
					{
						Debug(0, ("Too many clients"));
						break;	
					}
					if (connection > max_fd)	
					{
						max_fd = connection;	
					}
					if (i > max_index_for_ConnectionPool)	
					{
						max_index_for_ConnectionPool = i;	
					}
					
  	  						
      				}
      				
      				
	    		}
	    		
	    				
			
			for (i = 0; i <= max_index_for_ConnectionPool; i++)
      			{
      				
      				SocketConnection = this->ConnectionPool[i];
      				
      				if (SocketConnection < 0)
      				{
      					continue;
      				}
      				
				if (FD_ISSET (SocketConnection, &ReadSet))
	 			{
	 				Debug (3,("Acceptor - data from client readable" ));
		  			
		  			
		  			ret = readDataFromSocket(SocketConnection,1024,2,1,Message);
		  			if (ret == 1024)
		  			{
		  				Debug (1,("socket buffer full fd = <%d>" ,SocketConnection));
		  				//hex_dumper(1,"recv :",Message,ret);
		  				//ret = -1;
		  				
		  			}
;					if (ret > 0) 
         				{
           				 	Debug (2,("Acceptor - got data send back to client length = <%d>" ,ret));
		  				Length = ret;
		  				
		  				
            					ret = sendDataToSocket(SocketConnection,Message,Length,5,1);
            					if (ret != 0)
                				{
                   					Warning(("RequestHandler send  back to client () failed with status "));
                   					break ;
               					}  
               					
               				}
               				else if (ret <= 0)
               				{
               					
               					
               					
               					Debug (0,("Close client connection fd=<%d>",SocketConnection));
               					close (SocketConnection);
		    				FD_CLR (SocketConnection, &AllSet);
		    				
		    				this->ConnectionPool[i] = -1;
		    			}

					
	      			
	      			}

			
			} // end for
			
      			
      		}
      		
      		    	  
  	}/*end while */

  	Debug(0, ("dispatcher OK"));  	
  	
  	return 0;
}




 



/******************************************************************************/
/*                                                                            */
/*  S e n d D a t a T o S o c k e t                                           */
/*                                                                            */
/******************************************************************************/ 

int Acceptor::sendDataToSocket(int SocketEventFD,char *Message,int Length,int TimeOut=5,int WriteAttempts=1)
{
   
	Debug(9,( "sendDataToSocket init"));
   	struct timeval 	socket_TimeOut;
  	
	socket_TimeOut.tv_usec = 0;
	int write_maxfd ;
	int nready;
	
	write_maxfd = SocketEventFD;
	fd_set WriteSet;
	FD_ZERO(&WriteSet);
	FD_SET(SocketEventFD, &WriteSet);

	
	
	for (int i=0 ; i< WriteAttempts ; i++)
	
	{   				
		
		socket_TimeOut.tv_sec = TimeOut;
		socket_TimeOut.tv_usec = 0;
		nready = select(write_maxfd + 1,NULL,&WriteSet ,NULL,&socket_TimeOut);
			
    		if ( FD_ISSET(SocketEventFD, &WriteSet) ) 
		{           				
            		int ret = send(SocketEventFD, Message, Length,0);
            			
            		if (ret > 0)
            		{
            			Debug(9,( "sendDataToSocket ok"));
            			
            			return ret;
            		}
            		else

            		{
            			Debug(1,( "sendDataToSocket error"));

            			return 1;
            		}
            	
                 }
        }				

	Debug(1,( "sendDataToSocket error ,Time out !"));
 	
 	return -1;

}


	
/******************************************************************************/
/*                                                                            */
/*  R e a D a t a T o S o c k e t                                           */
/*                                                                            */
/******************************************************************************/


int Acceptor::readDataFromSocket(int SocketEventFD,int Length,int TimeOut,int ReadAttempts,char*Message)

{   
	
	Debug(9,( "readDataFromSocket init"));	
	
	struct timeval 	socket_TimeOut;
	socket_TimeOut.tv_usec = 0;
	
	
			
	
	int read_maxfd ;	
	int nready;	
	read_maxfd = SocketEventFD;
	fd_set readDataSet;
	
	FD_ZERO(&readDataSet);
	FD_SET(SocketEventFD, &readDataSet);
	
	for (int i=0 ; i < ReadAttempts ; i++)
	{   		
		
		//socket_TimeOut.tv_sec = TimeOut;		
		//socket_TimeOut.tv_usec = 0;
		
		socket_TimeOut.tv_sec = 0;		
		socket_TimeOut.tv_usec = 3000;		  
		
		nready = select(read_maxfd + 1,&readDataSet, NULL,NULL,&socket_TimeOut);
			
    		if ( FD_ISSET(SocketEventFD, &readDataSet) ) 
		{           	
			int ret = recv (SocketEventFD, (void *)Message, Length,0);

			Debug(7,( "Acceptor number of byte read = <%d>",ret));
  			if (ret <= 0)
  			{
  				
    				return ret;
  			}			
  			else			
  			{	
  				
  				return ret;			
  			}
            	}
        }

	Debug(9,( "readDataFromSocket ok"));
	
  	return -1;
  
}



/**********************************************************************************/   
void Acceptor:: handle_timeout()
{	
  	Debug(9, ("handle_timeout Init"));
 
  	Debug(9, ("handle_timeout OK"));
}


/**********************************************************************************


**********************************************************************************/   
void Acceptor::Close()
{    
  
  	Debug(1, ("Acceptor Close Init"));
  	
  	
  	Debug(1, ("close connected client init"));
  	int i;
  	for (i=0;i< FD_SETSIZE;i++)
	{
		if (this->ConnectionPool[i] > 0 )
		{
			Debug(1, ("close connected client <%d>",this->ConnectionPool[i]));
			close(this->ConnectionPool[i]);
		}
	}
  	
  	if (this->listenfd)
  	{
  		Debug(1, ("Acceptor Close : close listen this->listenfd=<%d>",this->listenfd));
  		close(this->listenfd);
  		this->listenfd = 0;
  	}
  	
  	Debug(1, ("Acceptor Close OK"));
 
}

/**********************************************************************************


**********************************************************************************/   
Acceptor::~Acceptor()
{    
  
  	Debug(9, ("Acceptor destructor Init"));
/*
  	if (this->listenfd )
  	{
  		Debug(1, ("Acceptor destructor : close listen this->listenfd=<%d>",this->listenfd));
  		close(this->listenfd);
  	}
*/
  	
  	Debug(9, ("Acceptor destructor OK"));
 
}

