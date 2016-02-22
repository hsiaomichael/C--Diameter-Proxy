#include "PCA_XMLParser.hpp"



PCA_XMLParser::PCA_XMLParser(string XMLString)
{
	XMLTag = XMLString;
}


/*############################################################################
# return the Section in the form <StartTag>XXX</EndTag>. 
# of a field given TAG
#############################################################################*/
string PCA_XMLParser::getTagSection(string StartTag,string EndTag)
{
	string NoneStr = "None";	
	char Msg[100];
	
	int start_position = XMLTag.find(StartTag);
	if (start_position == -1)
	{
		strcpy(Msg ,"GetTagSection -- StartTag Not Found !");
		printf("%s\n",Msg);
		
		return NoneStr;
	}
	
	int end_position = XMLTag.find(EndTag);	
	if (end_position == -1)
	{
		strcpy(Msg ,"GetTagSection -- StartTag Not Found !");
		printf("%s\n",Msg);		
		return NoneStr;
	}
	
	string TagSection = XMLTag.substr(start_position,end_position-start_position+EndTag.size()) ;
	return TagSection;
}

  	
/*#########################################################################
# return the value of a field given in the form <TAG>XXX</TAG>. 
# XXX will be returned in this case.
##########################################################################*/
	
string PCA_XMLParser::getTagValue(string XMLString)
{
	
	
	string NoneStr = "None";
	char Msg[100];
	
	string Tag = ">";
	int StartPos = XMLString.find(Tag);	
  	if (StartPos == -1 )	  
  	{
    	  strcpy(Msg ,"getTagValue -- StartTag Not Found !");
	  printf("%s\n",Msg);	  
	  return NoneStr;
    	}	
    	
    	  Tag = "<";
        int EndPos = XMLString.find(Tag,1);
  	if (EndPos == -1 )
    	{  		
    	  strcpy(Msg ,"getTagValue -- EndTag Not Found !");
	  printf("%s\n",Msg);
	  return NoneStr;
	}
  	
  	string TagSection = XMLString.substr(StartPos+1,EndPos-StartPos-1) ;
	
	
	return TagSection;
	
} 

/***********************************************************************************



***********************************************************************************/

string PCA_XMLParser::getXMLTagValue(string XMLString,string Tag)
{
	
	string NoneStr = "None";
	
	
	XMLTag = XMLString;
	
	string StartTag = "<" + Tag + ">";
	string EndTag = "</" + Tag + ">";
	
	string TagSection;
  	TagSection = getTagSection(StartTag,EndTag);
  	
  	string Field = getTagValue(TagSection);
  	
   	return Field;
}
   

/***********************************************************************************



***********************************************************************************/
   	

PCA_XMLParser::~PCA_XMLParser ()
{
	
	
}
