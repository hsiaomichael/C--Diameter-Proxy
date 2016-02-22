

#include "PCA_ProxySocket.hpp"
#include "PCA_XMLParser.hpp"
#include "PCA_GenLib.hpp"

int client_port = 0;
int server_port = 0;
char client_host_name[10];
char server_host_name[20];
string XMLCFG = "";
const char process_name[20] = "DiameterProxy";


void MainTest(string XMLCFG)
{
	try
	{
		try
		{
			int TimeOut = 5;
			DBXMLCFGInit(XMLCFG);
			InitialiseSignalHandling();
			
			
			ProxyAcceptor *ProxyServer = NULL;
	
    			ProxyServer = new ProxyAcceptor(XMLCFG);			
			try
			{
				ProxyServer->dispatcher(TimeOut);
    				ProxyServer->Close();
    				delete ProxyServer;
    
    				Debug(5, ("calling Acceptor ok "));

				Info(("The process is exiting ..."));
				
			}
			catch (exception &e)
			{	
				printf("Exception 1 : %s\n" , e.what ());	
				throw "Exception 1";
			}			
			CloseLog();
		}
		
		catch (exception &e)
		{
		
			printf("Exception 2: %s\n",e.what ());
			throw "Exception 2";
			
		}
		
	}
	catch(const char* str)
	{         
		printf("Exception ErrMsg: %s\n",str );
	} 
	catch (...)
	{
		
 	  	printf("Unknow Exception : End Of Process !\n");
	}
}


/******************************************************************


******************************************************************/
int main (int argc, char *argv[])
{
	char buffer[1024];
   	char InputFileName[100];
  	string line;
   	
 
   	
   	if (argc != 2)
   	{
     		printf("Usage : %s <configuration file name> \n", argv[0]);
     		return 1;
   	}
   	
   	if (0 != DbInit())
   	{
   		printf( "Failed to initialise log file.\n" );
     		return 1;
   	}
   	
   	sprintf(InputFileName,"%s",argv[1]);   	
   	
   	FILE *InputFile;         
   	if ( ( InputFile = fopen(InputFileName,"r") ) == NULL )
   	{	
		sprintf(InputFileName,"/usr/aethos/psa/current/bin/%s",argv[1]);
     		if ( ( InputFile = fopen(InputFileName,"r") ) == NULL )
      		{
   
        		printf("Error opening file <%s> ! \n",InputFileName);
        		DbTerm();
        		exit (1);
      		} 
   	}
   	
   	        
   	while (fgets(buffer,1024,InputFile) != NULL)
   	{                 
                
     		line = buffer;
     		XMLCFG = XMLCFG + line;
   	} 
   	fclose(InputFile);
   	
   	
   	MainTest(XMLCFG);
   	//printf("make sure all thread got terminated flag sleep 5 seconds before exit\n");
   	
   	sleep(8);
	
   	if (DbTerm() != 0 )
   	{
        	printf("Failed to terminate log file.\n" );
        	return 1;
   	}
}

		

