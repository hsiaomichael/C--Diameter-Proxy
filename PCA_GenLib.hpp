
#ifndef __PCA_GenLib__
#define __PCA_GenLib__

#include <stdlib.h>
#include <cstring>
#include <cstddef>

#include <iostream>

using namespace std;


#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/time.h>
/* remove it in HP-UX */
#include <sys/select.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <stdarg.h>
#include <unistd.h>
#include <vector>
#include <list>
#include <map>



#define OK 1
#define FAIL -1
#define FALSE 0
#define TRUE 1

int InitialiseSignalHandling ( void );


int DbInit();
int DbTerm();
void SetTerminateFlag(bool *flag);
int UnregisterThread();
bool GetTerminateFlag();
int RegisterThread(const char *Msg,bool *flag);

void HexDumper( int level, const char *name, const char *data, int len );
void hex_dumper( int level, const char *name, const char *data, int len );
void GetHexTID( char *data, int len );

void	WriteLog(int level, const char *fmt, ...);


extern int db_level;
extern const char process_name[20];

int DbOutput(const char *fstring, ... );
int DbPrepare(const char *file, int line,int code);




#define DBPrintf(code,params)	{\
					if (code <= db_level)\
					{\
						(void) DbPrepare(__FILE__,__LINE__,code);\
						(void) DbOutput params;\
					}\
				}

#define  Debug(LEVEL,PARAMS)  DBPrintf(LEVEL,PARAMS)
#define  Warning(PARAMS)      DBPrintf(0,PARAMS)
#define  Error(PARAMS)        DBPrintf(0,PARAMS)
#define  Info(PARAMS)         DBPrintf(0,PARAMS)

int DBXMLCFGInit(string XMLCFG);
int CloseLog();
int OpenLog();



#endif
