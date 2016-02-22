



#include "PCA_GenLib.hpp"
#include "PCA_XMLParser.hpp"
PCA_XMLParser *XMLParser;


static char mystring[4096];
int  log_level = 1;
FILE  *log_file=stdout;
FILE *fp = (FILE *)NULL;
string DebugLogFileName;
int db_level;
static int FlushFile;
static int PrintDebug;
static int FileLineNo = 0;
static int FileNameSeqNo = 0;	
static int print_debug_level = 0;
static bool terminate_flag = FALSE;
static  int MaxRecordInOneDebugLogFile = 10000;
char TimeString[512];
static char Msg[1024];
static pthread_mutex_t debug_mutex;
static const char *db_file;
static int db_line;

/***********************************************************************************



***********************************************************************************/

	
int DbPrepare(const char *file, int line,int code)
{
	
	if( pthread_mutex_lock( &debug_mutex ) )
	{
		printf("Error can not lock debug mutex \n");	
		SetTerminateFlag(NULL);	
	}
  	
	print_debug_level = code;
	db_file = file;
	db_line = line;
	return 0;
}
/***********************************************************************************



***********************************************************************************/


int  DbOutput(const char *fstring, ...)
{		
	
	
  	
		va_list	arg_ptr;

		va_start(arg_ptr,fstring);
		
		vsprintf(mystring,fstring,arg_ptr);
		
		struct timeb tp;
	  	ftime(&tp);
	  	int millitm = tp.millitm;
	  	int CurrentTime = tp.time;
	  	time_t clock = CurrentTime;
	  	
          	strftime(TimeString,sizeof(Msg),"%Y-%m-%d %H:%M:%S",localtime(&clock));
          	sprintf(Msg,"%s.%03d",TimeString,millitm);
          	sprintf(Msg,"%s %s[%d]DB%d-%s/%d ==> ",Msg,process_name,getpid(),print_debug_level,db_file,db_line);
          	
          	if ( fp != (FILE *)NULL )
		{
          		fprintf(fp, "%s", Msg);
          		fprintf(fp, "%s\n", mystring);
          		if (FlushFile == 1)
			{				
				fflush(fp);
			}
          	}
          	
		
		if (PrintDebug == 1)
		{		
			printf(Msg);
			printf(mystring);
			printf("\n");
		}
		
		
		va_end(arg_ptr);
		
		
		FileLineNo++;
		
		
		if (FileLineNo > MaxRecordInOneDebugLogFile)
		{
			FileLineNo = 0;
			char Arcihve_DebugFileName[512];		
			sprintf(Arcihve_DebugFileName,"%s.%d",DebugLogFileName.c_str(),FileNameSeqNo);
			FileNameSeqNo++;
			sprintf(Msg,"Debug Log File more than %d records , rename <%s> to <%s>",MaxRecordInOneDebugLogFile,DebugLogFileName.c_str(),Arcihve_DebugFileName);
			
			if ( fp != (FILE *)NULL )
			{
				fprintf(fp,"%s\n",Msg);
				fclose(fp);
			}
			
			
			
			if (remove(Arcihve_DebugFileName) == -1)
			
			
			if (rename(DebugLogFileName.c_str(),Arcihve_DebugFileName) == -1)
			{
				printf("rename file error\n");
				perror("error");
       			
			}	
		
		
			OpenLog();
			 
		}	
		
		
		
		if( pthread_mutex_unlock( &debug_mutex ) )
		{
			printf("Error can not unlock debug mutex \n");	
			SetTerminateFlag(NULL);	
		}
		
		
			
		return 1;
}


/***********************************************************************************



***********************************************************************************/

int OpenLog()
{     
	
   FileLineNo = 0;
   
   char debug_file_name[256];
   sprintf(debug_file_name,"%s",DebugLogFileName.c_str());
   
    
  
   fp = fopen(debug_file_name,"w+");   
   if (fp == NULL)
   {
   	printf("Error opening file <%s>",debug_file_name);
   	exit (1); 
   }
   
   fprintf(fp,"Open debug log file name = <%s>\n",debug_file_name);
          	
  return 0;
	
}

/***********************************************************************************



***********************************************************************************/
int CloseLog()
{
  
	Debug(1, ("PCA_GenLib CloseLog",0));
	
	
	if ( fp != (FILE *)NULL )
	{
		fprintf(fp,"close debug log\n");
   
		fclose(fp);
	}
	return 0;
}
		
/*############################################################
## Debug Log Function , provide level 0~9 debug log
## setup environment variable
## DB_PRINT : Print Debug Message to stdin
## DB_FLUSH : flush file descriptor
## DLVL_ProgramName : Debug Log Level
## DLOG_ProgramName : Debug Log Full File Name
############################################################*/
int DBXMLCFGInit(string XMLCFG)
{
	
	try
	{
   		if ( pthread_mutex_init ( &debug_mutex, NULL ) )
		{
			Error ( ( "pthread_mutex_init debug_mutex failed with %d  - shutdown process", errno ) );
			return errno;
		}
  	
  		
  		
  	
  	
  		FileNameSeqNo = 0;
		XMLParser = new PCA_XMLParser(XMLCFG);
		string Tag ;
		string TagValue;
  
		Tag = "DLOG";
		TagValue = XMLParser->getXMLTagValue(XMLCFG,Tag);
		DebugLogFileName = TagValue;
  
  
		Tag = "DLVL";
		TagValue = XMLParser->getXMLTagValue(XMLCFG,Tag);
		db_level = atoi(TagValue.c_str());
  
  
		Tag = "DB_FLUSH";
		TagValue = XMLParser->getXMLTagValue(XMLCFG,Tag);
		FlushFile = atoi(TagValue.c_str());
  
  	
		Tag = "DB_PRINT";
		TagValue = XMLParser->getXMLTagValue(XMLCFG,Tag);
		PrintDebug = atoi(TagValue.c_str());
	
  		// FileNameSeqNo++;
   		sprintf(Msg,"%s.%05d",DebugLogFileName.c_str(),getpid());
  		DebugLogFileName = Msg;
 
  
  		OpenLog();
  		delete XMLParser;
  		return 0;
  
  	
	}
  	catch (...)
    	{
		
      		Debug(1, ("BXMLCFGInit error "));
      		throw;
		
    	}
    	
   
   
}


/*############################################################
## Debug Log Function , provide level 0~9 debug log
## setup environment variable
## DB_PRINT : Print Debug Message to stdin
## DB_FLUSH : flush file descriptor
## DLVL_ProgramName : Debug Log Level
## DLOG_ProgramName : Debug Log Full File Name
############################################################
*/

void	TimeStamp(FILE *out) {
    time_t		t;
	struct tm	*result;
	
	if (out == 0)
		out	= stdout;
	
	t			= time(0);
	result		= localtime(&t);
	
	
	if (result->tm_year > 100) {
		result->tm_year -= 100;
		if (result->tm_year > 10)
			fprintf(out, "[20%d-", result->tm_year);
		else
			fprintf(out, "[200%d-", result->tm_year);
	}
	else
		fprintf(out, "[19%d-", result->tm_year);
	
	
	
	if (result->tm_mon < 9)
		fprintf(out, "0%d-", result->tm_mon+1);
	else
		fprintf(out, "%d-", result->tm_mon+1);
	
	
	
	if (result->tm_mday < 10)
		fprintf(out, "0%d ", result->tm_mday);
	else
		fprintf(out, "%d ", result->tm_mday);
	
	
	
	if (result->tm_hour < 10)
		fprintf(out, "0%d:", result->tm_hour);
	else
		fprintf(out, "%d:", result->tm_hour);
	
	if (result->tm_min < 10)
		fprintf(out, "0%d:", result->tm_min);
	else
		fprintf(out, "%d:", result->tm_min);
	
	if (result->tm_sec < 10)
		fprintf(out, "0%d]\t", result->tm_sec);
	else
		fprintf(out, "%d]\t", result->tm_sec);
}

/***********************************************************************************



***********************************************************************************/

void	WriteLog(int level, const char *fmt, ...)
{
	va_list	args;
	
	if (level <= log_level)
	{
		va_start(args, fmt);
		TimeStamp(0), fprintf(log_file, "DB%d", level), vfprintf(log_file, fmt, args);
		va_end(args);
		if (log_file != stdout)
			fflush(log_file);
	}
}


/**************************************************************/
int DbInit()
{
  
  return 0;
}

/**************************************************************/
int DbTerm()
{
  
  
  
  pthread_mutex_destroy ( &debug_mutex );
  
  return 0;
}

/***********************************************************************************



***********************************************************************************/

void SetTerminateFlag ( bool *flag )
{
  
  
  pthread_mutex_lock( &debug_mutex );
  
  if ( flag )
  {
    *flag = TRUE;
  }
  else
  {
    terminate_flag = TRUE;
  }
  
  
  pthread_mutex_unlock( &debug_mutex );
  return;
} 
/***********************************************************************************



***********************************************************************************/

int UnregisterThread()
{
	return 1;
}

/***********************************************************************************



***********************************************************************************/

bool GetTerminateFlag(void)
{
 
  bool  temp_b = FALSE;
 pthread_mutex_lock(&debug_mutex);
   
  
  temp_b = terminate_flag;
  
  pthread_mutex_unlock( &debug_mutex );
  return temp_b;
   
} /* bool GetTerminateFlag ( bool *flag ) */


int RegisterThread(const char *Msg,bool *flag)
{
	return 0;
}
/**************************************************************/

void GetHexTID( char *data, int len )
{
  char HexStr[17];
  unsigned char buf [ 9 ];
 
  
  memset ( buf, 0xff, sizeof buf );
  memcpy ( buf, data, len );
     
      sprintf(HexStr, "%02x%02x%02x%02x%02x%02x%02x%02x",
                     buf [ 0 ],
                     buf [ 1 ],
                     buf [ 2 ],
                     buf [ 3 ],
                     buf [ 4 ],
                     buf [ 5 ],
                     buf [ 6 ],
                     buf [ 7 ]
                     );
    
   
  	memcpy(data, HexStr,16);
  return;
} /* void HexDumper ( int level, const ca=har *name, const void *data, size_t len ) */


/***********************************************************************************



***********************************************************************************/

void HexDumper( int level, const char *name, const char *data, int len )
{
  char Msg[1024];
  int count=0;
  unsigned char buf [ 1024 ];
 
  memset ( buf, 0xff, sizeof buf );
  memcpy ( buf, data, len );

  sprintf(Msg,"%s ( length %d bytes )", name, len );
 
 
  for ( count = 0; count <= len / 8; count++ )
  {
     /*
     printf( "%02x %02x %02x %02x %02x %02x %02x %02x \n",
                     buf [ ( count * 8 ) + 0 ],
                     buf [ ( count * 8 ) + 1 ],
                     buf [ ( count * 8 ) + 2 ],
                     buf [ ( count * 8 ) + 3 ],
                     buf [ ( count * 8 ) + 4 ],
                     buf [ ( count * 8 ) + 5 ],
                     buf [ ( count * 8 ) + 6 ],
                     buf [ ( count * 8 ) + 7 ]
                     );
                    */
      sprintf(Msg, "%02x %02x %02x %02x %02x %02x %02x %02x ",
                     buf [ ( count * 8 ) + 0 ],
                     buf [ ( count * 8 ) + 1 ],
                     buf [ ( count * 8 ) + 2 ],
                     buf [ ( count * 8 ) + 3 ],
                     buf [ ( count * 8 ) + 4 ],
                     buf [ ( count * 8 ) + 5 ],
                     buf [ ( count * 8 ) + 6 ],
                     buf [ ( count * 8 ) + 7 ]
                     );
    
      if ( level <= 1 )
     		printf("%s : %s\n",name,Msg);
  }
  return;
} /* void HexDumper ( int level, const ca=har *name, const void *data, size_t len ) */



/***********************************************************************************



***********************************************************************************/

void hex_dumper( int level, const char *name, const char *data, int len )
{
	
	 if ( level <= 1 )
	 {
  		char Msg[1024];
  		int count=0;
  		unsigned char buf [ 1024 ];
 
  		memset ( buf, 0xff, sizeof buf );
  		memcpy ( buf, data, len );

  		sprintf(Msg,"%s ( length %d bytes )", name, len );
 
 
  		for ( count = 0; count <= len / 8; count++ )
  		{
     
     
     	 		sprintf(Msg, "%02x %02x %02x %02x %02x %02x %02x %02x ",
                    	buf [ ( count * 8 ) + 0 ],
                     	buf [ ( count * 8 ) + 1 ],
                     	buf [ ( count * 8 ) + 2 ],
                     	buf [ ( count * 8 ) + 3 ],
                     	buf [ ( count * 8 ) + 4 ],
                     	buf [ ( count * 8 ) + 5 ],
                     	buf [ ( count * 8 ) + 6 ],
                     	buf [ ( count * 8 ) + 7 ]
                    	);
    
     			//printf("%s : %s\n",name,Msg);
     			Debug(level, ("%s : %s",name,Msg));
 
  
 		}
 	}
 	 return;
 
} /* void HexDumper ( int level, const ca=har *name, const void *data, size_t len ) */


/***************************************************************************
 * Local function prototypes
 ***************************************************************************/

//static void  signal_handler ( int sig );
static void *signal_waiter  ( void *arg );

/***************************************************************************
 * Static data
 ***************************************************************************/

static sigset_t   signal_set;


/***************************************************************************
 * Main source
 ***************************************************************************/

int InitialiseSignalHandling ( void )
{
  int       status;
  pthread_t id;
  
  
  Debug ( 1, (  "InitialiseSignalHandling init" ));
  /* Initialise the signal set */
  if ( sigemptyset ( &signal_set ) ||
       sigaddset ( &signal_set, SIGTERM ) ||
       sigaddset ( &signal_set, SIGQUIT ) ||
       sigaddset ( &signal_set, SIGINT ) ||
       sigaddset ( &signal_set, SIGHUP )  ||
       sigaddset ( &signal_set, SIGUSR1 ) ||
       sigaddset ( &signal_set, SIGUSR2 ) )
  {
     Error ( (  "Error setting up signal mask" ));
     return FAIL;
  }
  /* Set all threads to block the above signals */
  status = sigprocmask ( SIG_SETMASK, &signal_set, NULL );
  if (status != 0)
  {
     Error ( (  "Unable to set signal behaviour to block: %d",   errno  ));
     return FAIL;
  }

  status = pthread_create ( &id, NULL,signal_waiter, NULL ) ;
  if (status != 0)
  {
     Error ( ( "Unable to create signal handling thread: %d",errno  ) );
     return FAIL;
  }
  Debug ( 1, (  "InitialiseSignalHandling ok" ));
  return OK;
} 

/***********************************************************************************



***********************************************************************************/

static void *signal_waiter ( void *arg )
{
  int        err,sig;
  int        status;
  //pthread_t  id = pthread_self ();
       
  RegisterThread ( "sighandler", NULL );
        
  Debug ( 1, (  "Signal handling init" ));
  //if ( status = pthread_detach ( pthread_self() ) )
  
  //if ( status = pthread_detach ( pthread_self() ) )
  status = pthread_detach ( pthread_self() ) ;
  if (status != 0)
  {
     Error ( ( "Unable to detach signal handling thread from main process: %d", errno  ));
  }
  while ( ! GetTerminateFlag () )
  {
     /*printf ("looping ......\n");*/
     err = sigwait ( &signal_set, &sig );
     switch ( sig )
     {
        case SIGTERM:
        case SIGQUIT:
        case SIGINT:
               Info ( ( "Received signal %d, scheduling process termination", sig ));
               SetTerminateFlag ( NULL );
               break;
        case SIGHUP:
               Info ( ( "Received signal %d, scheduling configuration re-load", sig ));
               break;
        case SIGUSR1:
               if ( db_level < 9 )
               {
                  db_level++;
               }
               Info ( ("Debug Level Changed to %d", db_level ));
               break;
        case SIGUSR2:
               if( db_level > -3 )
               {
                  db_level--;
               }
               Info ( ( "Debug Level Changed to %d", db_level ));
               break;
        default:
               Info ( ( "sigwait returned signal that wasn't trapped (%d)", sig ));
               SetTerminateFlag ( NULL );
               break;
     }
   }
   Debug ( 1, (  "Signal handling ok" ));
   UnregisterThread();
   return NULL;
   
} /* static void *signal_waiter ( void *arg ) */



