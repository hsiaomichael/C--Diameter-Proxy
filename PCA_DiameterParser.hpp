
#ifndef __PCA_DiameterParser___
#define __PCA_DiameterParser___


#include "PCA_GenLib.hpp"
#include "PCA_XMLParser.hpp"

class DiameterParser
{
	public:

  		
  		DiameterParser();
  		
  		int parse(const char *Direction,char *client_info,char *SocketMessage,int SocketMessage_length);
  		int parseAVP(char *AVP,int length_of_AVP);
  		
 
  
	public:
    		
    		char DiameterMessage[1024];
   
   
};

#endif






