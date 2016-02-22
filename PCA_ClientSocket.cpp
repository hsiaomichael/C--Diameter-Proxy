#include "PCA_ClientSocket.hpp"

/******************************************************************************

******************************************************************************/


Connector::Connector(string XMLCFG)
{
	
  try
  {
	
	Debug(9, ("Connector constructor Init"));
	
	this->IsConnected = 0;
	this->bind_port = -1;
	
	PCA_XMLParser *XMLParser = NULL;
	string Tag;
   	string TagValue;
    	char *tail;
  	

   	XMLParser = new PCA_XMLParser(XMLCFG);
        
        
   	Tag = "REMOTE_HOST";
   	TagValue = XMLParser->getXMLTagValue(XMLCFG,Tag);
   	Debug(2, ("%s=%s",Tag.c_str(),TagValue.c_str()));
   	strcpy(this->host,TagValue.c_str());
   	
   	
   	Tag = "CONNECT_PORT";
   	TagValue = XMLParser->getXMLTagValue(XMLCFG,Tag);
   	Debug(2, ("%s=<%s>",Tag.c_str(),TagValue.c_str()));
   	this->port = strtol(TagValue.c_str(), &tail, 10);
   
   
   	delete XMLParser;  
    	
 	
  }
  catch (...)
  {
  	Debug(0, ("Connector constructor Error"));
  	throw "Connector constructor Error";
  }
   
}


/******************************************************************************/
/*                                                                            */
/* Server Dispatcher				                              */
/*                                                                            */
/******************************************************************************/
 
int Connector::Connect()
{     
	try
	{
		int RC=0;
	
		Debug(1, ("Connector init"));  	
		
		
		struct sockaddr_in SocketAddress;
		struct in_addr IPAddress;            /* Used to hold INADDR_ANY */
		IPAddress.s_addr = INADDR_ANY;
		memset (&SocketAddress, 0, sizeof SocketAddress);
		memcpy (&SocketAddress.sin_addr,&IPAddress,sizeof IPAddress);
	
		SocketAddress.sin_port =  htons(this->bind_port);
		SocketAddress.sin_family = AF_INET;
		SocketAddress.sin_addr.s_addr = inet_addr(this->host);

	
		/*-----Create the advertising socket-----*/
		Debug(2, ("Create the listen socket"));
		this->SocketDescriptor = socket (AF_INET, SOCK_STREAM, 0);
		if (this->SocketDescriptor < 0)
		{   
			Debug(1, ("Failed to create listen socket - errno = %d (%s)",errno, strerror(errno) ));
			RC = -1;
		}
	
		Debug(2, ("Set SO_REUSEADDR server socket optiont"));
		static const int  NON_ZERO = 1;	
		if (setsockopt(this->SocketDescriptor,SOL_SOCKET,SO_REUSEADDR,&NON_ZERO,sizeof NON_ZERO) < 0)
		{  
			Debug(1, ("Failed to set listen socket option SO_REUSEADDR - errno = %d (%s)",errno, strerror(errno) ));
			close(this->SocketDescriptor);
    			RC = -1;
  		}
  	
  		Debug(2, ("Set SO_KEEPALIVE server socket option")); 
		if (setsockopt(this->SocketDescriptor,SOL_SOCKET,SO_KEEPALIVE,&NON_ZERO,sizeof NON_ZERO ) < 0)
  		{  
     			Debug(1,("Failed to set listen socket option SO_KEEPALIVE on advertising socket - errno = %d (%s)",errno, strerror(errno)));
     			close(this->SocketDescriptor);
     			RC = -1;
  		}
  	
  		Debug(2, ("Set SO_LINGER listen socket option to discard data on close")); 
		struct linger LingerData;
		LingerData.l_onoff  = 0;
		LingerData.l_linger = 0;
  		if (setsockopt(this->SocketDescriptor,SOL_SOCKET,SO_LINGER,&LingerData,sizeof LingerData ) < 0)
		{     
			Debug(0,( "Failed to set listen socket option SO_LINGER on advertising socket - errno = %d (%s)",errno, strerror(errno)));
     
			close(this->SocketDescriptor);
 			RC = -1;
  		}
   	
   		if (this->bind_port != -1)
   		{
  			Debug(2, ("bind")); 
    	
    			if (bind(this->SocketDescriptor,(struct sockaddr *) &SocketAddress,sizeof SocketAddress) < 0)
    			{      
      				Debug(1, ("bind() failed on listen socket - errno = %d (%s)",errno, strerror(errno)));
      
      				close(this->SocketDescriptor);
      				RC = -1;
    			}
    		}
    	
  
		Debug(1,( "Socket init OK, No errors so far socket fd = <%d>",this->SocketDescriptor));
  	 
		Debug(9, ("Connector constructor ok"));
	
		if (RC == -1)
		{
		
  			Debug(1, ("init socket error"));
  			throw "init socket error";
		}
		
		
		
        	Debug(1, ("Remote Host=<%s>,Port=<%d>,Bind Port=<%d>",this->host,this->port,this->bind_port));
        	
        	memset (&SocketAddress, 0, sizeof SocketAddress);
		memcpy (&SocketAddress.sin_addr,&IPAddress,sizeof IPAddress);
	
		SocketAddress.sin_port =  htons(this->port);
		SocketAddress.sin_family = AF_INET;
		SocketAddress.sin_addr.s_addr = inet_addr(this->host);
	
	
		while (GetTerminateFlag () != TRUE)
		{
		
			RC = connect (this->SocketDescriptor, (struct sockaddr *) &SocketAddress, sizeof SocketAddress);
			if ((RC < 0) && (errno != EINTR))
			{
       
				
				Debug(0, ("RC=<%d>,Failed to connect client socket Remote Host=<%s>,Port=<%d>,Bind Port=<%d>",RC,this->host,this->port,this->bind_port));
        			Debug(0, ("Failed to connect client socket - errno = %d (%s)",errno, strerror(errno)));
				
				this->IsConnected = 0;
				break;
			}
			else
			{
				
				//Debug(-, ("Connect to server - connection created - socket descriptor socket descriptor = %d",this->SocketDescriptor));
				Debug(0, ("Connect to server - connection created , Remote Host=<%s>,Port=<%d>,Bind Port=<%d>,socket_fd=<%d>",this->host,this->port,this->bind_port,this->SocketDescriptor));
        			
				this->IsConnected = 1;
				break;
	
			}
		}

  		Debug(2, ("Connector ok"));  
  		
  		
  		return this->IsConnected;
  	}
  	catch(...)
  	{
  		throw "Can not connect to remote host";
  	}
  	
}




 

/******************************************************************************/
/*                                                                            */
/*  S e n d D a t a T o S o c k e t                                           */
/*                                                                            */
/******************************************************************************/ 

int Connector::sendDataToSocket(char *Message,int Length,int TimeOut=5,int WriteAttempts=1)
{
   
	Debug(9,( "sendDataToSocket init"));
	
	
	if (! this->IsConnected)
	{
		return -2;	
	}
   	struct timeval 	socket_TimeOut;
  	
	socket_TimeOut.tv_usec = 0;
	int write_maxfd ;
	int nready;
	
	write_maxfd = this->SocketDescriptor;
	fd_set WriteSet;
	FD_ZERO(&WriteSet);
	FD_SET(this->SocketDescriptor, &WriteSet);

	/* new SocketConnectionPool connection */
	
	for (int i=0 ; i< WriteAttempts ; i++)
	
	{   				
		
		socket_TimeOut.tv_sec = TimeOut;
		socket_TimeOut.tv_usec = 0;

		nready = select(write_maxfd + 1,NULL,&WriteSet ,NULL,&socket_TimeOut);
			
    		if ( FD_ISSET(this->SocketDescriptor, &WriteSet) ) 
		{           				
            		//int ret = write(SocketEventFD, Message, Length);
            		
            		int ret = send(this->SocketDescriptor, Message, Length,0);
            			
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


int Connector::readDataFromSocket(int Length,int TimeOut,int ReadAttempts,char*Message)

{   
	
	Debug(9,( "readDataFromSocket init"));	
	
	
	if (! this->IsConnected)
	{
		return -2;	
	}
	
	struct timeval 	socket_TimeOut;
	socket_TimeOut.tv_usec = 0;		
	
	
	
	int read_maxfd ;	
	int nready;	
	read_maxfd = this->SocketDescriptor;
	fd_set readDataSet;
	
	FD_ZERO(&readDataSet);
	FD_SET(this->SocketDescriptor, &readDataSet);
	
	for (int i=0 ; i < ReadAttempts ; i++)
	{   		
		
		//socket_TimeOut.tv_sec = TimeOut;		
		//socket_TimeOut.tv_usec = 0;	
		
		socket_TimeOut.tv_sec = 0;		
		socket_TimeOut.tv_usec = 3000;		  
			  
		
		nready = select(read_maxfd + 1,&readDataSet, NULL,NULL,&socket_TimeOut);
			
    		if ( FD_ISSET(this->SocketDescriptor, &readDataSet) ) 
		{           	
			int ret = recv (this->SocketDescriptor, (void *)Message, Length,0);

			Debug(7,( "Connector number of byte read = <%d>",ret));
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

	Debug(9,( "readDataFromSocket ,Time out "));
	
  	return -1;
  
}



/**********************************************************************************


**********************************************************************************/   
void Connector:: handle_timeout()
{	
  	Debug(9, ("handle_timeout Init"));
 
  	Debug(9, ("handle_timeout OK"));
}


/**********************************************************************************


**********************************************************************************/   
void Connector::Close()
{    
  
  	Debug(2, ("Connector Close Init"));
  	
  	
  	if ( this->IsConnected)
  	{
  		if (this->SocketDescriptor )
  		{
  			Debug(0, ("Connector Close : close SocketDescriptor=<%d>",this->SocketDescriptor));
  			close(this->SocketDescriptor);
  			this->SocketDescriptor = 0;
  		}
  	  this->IsConnected = 0;
  	}
  	
  	Debug(2, ("Connector Close OK"));
 
}

/**********************************************************************************


**********************************************************************************/   
Connector::~Connector()
{    
  
  	Debug(3, ("Connector destructor Init"));
  	
  	
  	
  	
  	Debug(3, ("Connector destructor OK"));
 
}

