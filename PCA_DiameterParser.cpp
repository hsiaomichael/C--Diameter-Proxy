#include "PCA_DiameterParser.hpp"
 


DiameterParser::DiameterParser()
{    
  	
  	
 	Debug(9, ("DiameterParser::constructor Init"));
 	
 	memset(this->DiameterMessage,0x00,1024);
 
		
 	Debug(9, ("DiameterParser::constructor Ok"));
 	
} 




/*
			##################################################################
			#     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
			#    +_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+
			#    |                           AVP Code                            |
			#    +_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+
			#    |V M P r r r r r|                  AVP Length                   |
			#    +_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+
			#    |                        Vendor_ID (opt)                        |
			#    +_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+
			#    |    Data ...
			#    +_+_+_+_+_+_+_+_+
			#	
			##################################################################
*/
int DiameterParser::parseAVP(char *AVP,int length_of_AVP)
{
	
	 Debug(9, ("DiameterParser::parseAVP Init"));
	 
	 //Debug(1, ("DiameterParser::AVP_length=<%d>",AVP_length));
	 
	 int i;
	 int avp_code;
	 int avp_length;
	 char avp_length_message[4];
	 memset(avp_length_message,0x00,4);
	 char *pointer_to_AVP;
	 pointer_to_AVP = AVP;
	 
	 char avp_data[128];
	 int avp_data_int=0;
	 
	 int pos_AVP = 0;
	 
	 char avp_code_dict[100];  
	 
	 char avg_message[128];
	

	 for (i=0;i<1000;i++)
	 {
	 	if (length_of_AVP < 12)
	 		break;
	 		
	 		//Debug(1, ("LOOP=<%d>,length_of_AVP=<%d>",i,length_of_AVP));
	 
	 		 memcpy(&avp_code,pointer_to_AVP,4);
	 		 memcpy(&avp_length_message[1],pointer_to_AVP+5,3);   
	 		 memcpy(&avp_length,avp_length_message,4);
	 		 memset(avp_data,0x00,128);
	 		 memcpy(&avp_data,pointer_to_AVP+8,avp_length-8);
	 		 
	 		 
			switch( avp_code )
			{
				case 258 :strcpy(avp_code_dict,"AUTH_APPLICATION_ID"                     );
					memcpy(&avp_data_int,avp_data,avp_length-8);
					avp_data_int = ntohl(avp_data_int);
					sprintf(avg_message,"<%s>=<%d> ",avp_code_dict,avp_data_int);
	 				strcat(this->DiameterMessage,avg_message);
					break;
				case 278 :strcpy(avp_code_dict,"ORIGIN_STATE_ID"                     );
					memcpy(&avp_data_int,avp_data,avp_length-8);
					avp_data_int = ntohl(avp_data_int);
					sprintf(avg_message,"<%s>=<%d> ",avp_code_dict,avp_data_int);
	 				strcat(this->DiameterMessage,avg_message);
					break;
				case 415 :strcpy(avp_code_dict,"CC_REQUEST_NUMBER"                    );
					memcpy(&avp_data_int,avp_data,avp_length-8);
					avp_data_int = ntohl(avp_data_int);
					sprintf(avg_message,"<%s>=<%d> ",avp_code_dict,avp_data_int);
	 				strcat(this->DiameterMessage,avg_message);
					break;
				case 416 :strcpy(avp_code_dict,"CC_REQUEST_TYPE"                    );
					memcpy(&avp_data_int,avp_data,avp_length-8);
					avp_data_int = ntohl(avp_data_int);
					sprintf(avg_message,"<%s>=<%d> ",avp_code_dict,avp_data_int);
	 				strcat(this->DiameterMessage,avg_message);
					break;
				case 485 :strcpy(avp_code_dict,"ACCOUNTING_RECORD_NUMBER"                    );
					
					memcpy(&avp_data_int,avp_data,avp_length-8);
					avp_data_int = ntohl(avp_data_int);
					sprintf(avg_message,"<%s>=<%d> ",avp_code_dict,avp_data_int);
	 				strcat(this->DiameterMessage,avg_message);
					break;
				
				case 480 :strcpy(avp_code_dict,"RECORD_TYPE"                    );
					
					memcpy(&avp_data_int,avp_data,avp_length-8);
					avp_data_int = ntohl(avp_data_int);
					sprintf(avg_message,"<%s>=<%d> ",avp_code_dict,avp_data_int);
	 				strcat(this->DiameterMessage,avg_message);
					break;
				
     

				case 263 :strcpy(avp_code_dict,"SESSION_ID"                     );
					sprintf(avg_message,"<%s>=<%s> ",avp_code_dict,avp_data);
	 				strcat(this->DiameterMessage,avg_message);
					break;
	 				break;
	 				
				case 264 :strcpy(avp_code_dict,"ORIGIN_HOST"                     );
					sprintf(avg_message,"<%s>=<%s> ",avp_code_dict,avp_data);
	 				strcat(this->DiameterMessage,avg_message);
					break;
				case 266 :strcpy(avp_code_dict,"VENDOR_ID"                     );
					sprintf(avg_message,"<%s>=<%s> ",avp_code_dict,avp_data);
	 				strcat(this->DiameterMessage,avg_message);
					break;
				case 268 :strcpy(avp_code_dict,"RESULT_CODE"                     );
					memcpy(&avp_data_int,avp_data,avp_length-8);
					avp_data_int = ntohl(avp_data_int);
					sprintf(avg_message,"<%s>=<%d> ",avp_code_dict,avp_data_int);
	 				strcat(this->DiameterMessage,avg_message);
					break;
				case 269 :strcpy(avp_code_dict,"PRODUCT_NAME"                     );
					sprintf(avg_message,"<%s>=<%s> ",avp_code_dict,avp_data);
	 				strcat(this->DiameterMessage,avg_message);
					break;
				
				case 281 :strcpy(avp_code_dict,"ERROR_MESSAGE"                     );
					sprintf(avg_message,"<%s>=<%s> ",avp_code_dict,avp_data);
	 				strcat(this->DiameterMessage,avg_message);
					break;
				
				case 283 :strcpy(avp_code_dict,"DESTINATION_REALM"                    );
					sprintf(avg_message,"<%s>=<%s> ",avp_code_dict,avp_data);
	 				strcat(this->DiameterMessage,avg_message);
					break;
				case 293 :strcpy(avp_code_dict,"DESTINATION_HOST"                    );
					sprintf(avg_message,"<%s>=<%s> ",avp_code_dict,avp_data);
	 				strcat(this->DiameterMessage,avg_message);
					break;
				
				case 296 :strcpy(avp_code_dict,"ORIGIN_REAM"                    );
					sprintf(avg_message,"<%s>=<%s> ",avp_code_dict,avp_data);
	 				strcat(this->DiameterMessage,avg_message);
					break;
				

				
				default :
					sprintf(avp_code_dict,"%s:%d","unknow avp",avp_code);
					sprintf(avg_message,"<%s>=<%s> ",avp_code_dict,avp_data);
	 				strcat(this->DiameterMessage,avg_message);
				break;
			}/* end switch */
   
   			
	 		 //Debug(1, ("avp_code=<%d>,avp_length=<%d>,avp_data=<%s>",avp_code,avp_length,avp_data));
	 		 
	 		
	 		 
	 		 avp_length = avp_length - 8;
	 		 
	 		 pos_AVP = pos_AVP + 8 + avp_length;
	 		 pointer_to_AVP = &AVP[pos_AVP];	 			 
	 		 length_of_AVP = length_of_AVP - 8 - avp_length;
	 		 
	 		 
	 	
	 	
	 }
	 
	 
	 Debug(9, ("DiameterParser::parseAVP OK"));
}



/*
				########################################################################
				#
				#   Diameter Header
				#
   				#    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   				#   +_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+
   				#   |    Version    |                 Message Length                |
   				#   +_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+
   				#   | command flags |                  Command_Code                 |
   				#   +_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+
   				#   |                         Application_ID                        |
   				#   +_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+
   				#   |                      Hop_by_Hop Identifier                    |
   				#   +_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+
   				#   |                      End_to_End Identifier                    |
   				#   +_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+
   				#   |  AVPs ...
   				#   +_+_+_+_+_+_+_+_+_+_+_+_+_
   				###############################################################################
				
*/

int DiameterParser::parse(const char *Direction,char *client_info,char *SocketMessage,int SocketMessage_length)

{    
  
  char data_information[100];
  char *diameter_message;
  
  
  
  Debug(9, ("DiameterParser::parse Init"));
  
  memset(this->DiameterMessage,0x00,1024);
 
  
  sprintf(data_information,"Recv from %s id=<%s>, length=<%d>",Direction,client_info,SocketMessage_length);
  Debug(1, ("%s",data_information));
 
  //Debug(1, ("Recv from %s id=<%s>, length=<%d>",Direction,client_info,SocketMessage_length));
  
  hex_dumper(2,data_information,SocketMessage,SocketMessage_length);
 
  diameter_message = SocketMessage;
  
  
  unsigned char version;
  
  version = *diameter_message;
  
  
  
  int diameter_length;  
  char Message_Length[4];
  memset(Message_Length,0x00,4);	 						
  memcpy( &Message_Length[1],diameter_message+1,3);
  memcpy(&diameter_length,Message_Length,4);  
  diameter_length = ntohl(diameter_length);
  
  unsigned char command_flags;
  command_flags = *(diameter_message+4)	;				
  
  
  char flag_r[4];
  if ( command_flags && 0x80 ) 	
  {
  	strcpy(flag_r,"REQ");
 
  }
  else
  {
  	strcpy(flag_r,"ANS");
  }
 
  int command_code;
  char Message_Command_Code[4];
  memset(Message_Command_Code,0x00,4);
  memcpy( &Message_Command_Code[1],diameter_message+5,3);
  memcpy(&command_code,Message_Command_Code,4);  
  command_code = ntohl(command_code);
  
  char command_code_dict[100];  
  switch( command_code )
  {
     case 265 :strcpy(command_code_dict,"AA"                     );  break;
     case 268 :strcpy(command_code_dict,"EAP"                    );  break;
     case 274 :strcpy(command_code_dict,"Abort_Session"          );  break;
     case 271 :strcpy(command_code_dict,"Accounting"             );  break;
     case 272 :strcpy(command_code_dict,"Credit_Control"         );  break;
     case 257 :strcpy(command_code_dict,"Capabilities_Exchange"  );  break;
     case 280 :strcpy(command_code_dict,"Device_Watchdog"        );  break;
     case 282 :strcpy(command_code_dict,"Disconnect_Peer"        );  break;
     case 258 :strcpy(command_code_dict,"Re_Auth"                );  break;
     case 275 :strcpy(command_code_dict,"Session_Termination"    );  break;
     case 300 :strcpy(command_code_dict,"User-Authorization"     );  break;
     case 301 :strcpy(command_code_dict,"Server-Assignment"      );  break;
     case 302 :strcpy(command_code_dict,"Location-Info"          );  break;
     case 303 :strcpy(command_code_dict,"Multimedia-Auth"        );  break;
     case 304 :strcpy(command_code_dict,"Registration-Termination");  break;
     case 305 :strcpy(command_code_dict,"Push-Profile"           );  break;
     case 306 :strcpy(command_code_dict,"User-Data"              );  break;
     case 307 :strcpy(command_code_dict,"Profile-Update"         );  break;
     case 308 :strcpy(command_code_dict,"Subscribe-Notifications");  break;
     case 309 :strcpy(command_code_dict,"Push-Notification"      );  break;
     case 310 :strcpy(command_code_dict,"Bootstrapping-Info"     );  break;
     case 311 :strcpy(command_code_dict,"Message-Process"        );  break;
 	
  default :
       strcpy(command_code_dict,"unknow command code");  
       break;
   }/* end switch */
  
 
 
 
  int application_id; 	 						
  memcpy(&application_id,diameter_message+8,4);
  application_id = ntohl(application_id);
  				  
  
  char application_id_dict[100];  
  switch( application_id )
  {
     case 0 :strcpy(application_id_dict,"DIAMETER_COMMON_MESSAGES");  break;
     case 1 :strcpy(application_id_dict,"NASREQ");  break;
     case 2 :strcpy(application_id_dict,"MOBILE_IP");  break;
     case 3 :strcpy(application_id_dict,"DIAMETER_BASE_ACCOUNTING");  break;
 	
     default :
       strcpy(application_id_dict,"unknow application id");  
       break;
   }/* end switch */
  
 	 
  
  int Hop_by_Hop_ID; 	 						
  memcpy(&Hop_by_Hop_ID,diameter_message+12,4);
  Hop_by_Hop_ID = ntohl(Hop_by_Hop_ID);
  
  int End_to_End_ID; 	 						
  memcpy(&End_to_End_ID,diameter_message+16,4);
  End_to_End_ID = ntohl(End_to_End_ID);
  
  
  parseAVP(diameter_message+20,SocketMessage_length-20);
  
  Debug(1, ("Version=<%d>,length=<%d>,command_flags=<%d>,flag_r=<%s>,command_code=<%d:%s>,application_id=<%d:%s>,Hop_by_Hop_ID=<%d>,End_to_End_ID=<%d>,AVP = %s",version,diameter_length,command_flags,flag_r,command_code,command_code_dict,application_id,application_id_dict,Hop_by_Hop_ID,End_to_End_ID,this->DiameterMessage));
 
  
  Debug(9, ("DiameterParser::parse Ok"));
  return 0;
}


