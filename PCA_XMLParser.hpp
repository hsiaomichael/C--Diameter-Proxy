
#ifndef __PCA_XMLParser__
#define __PCA_XMLParser__

#include "PCA_GenLib.hpp"
class PCA_XMLParser 
{
  public:
    PCA_XMLParser(string XMLString);
    ~PCA_XMLParser();

    string getTagSection(string StartTag,string EndTag);
    
    string getTagValue(string XMLString);
    string getXMLTagValue(string XMLString,string Tag);
    
  private:
    string XMLTag;

};

#endif
