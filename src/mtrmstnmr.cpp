//============================================================================
// Name        : mtrmstnmr.cpp
// Author      : Mark Meadows
// Version     : 00.00.01.1
// Copyright   : Copyright (c) 2019 Mark Meadows
// Description : Mattermost Network Monitor in C++, Ansi-style
//============================================================================

#include <iostream>
#include <string>
#include <stdio.h>
#include <curl/curl.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
//#include <string.h>
#include <cstring>

using namespace std;

char LogLocation[250] = {};   //A place to store the host file
char WebHookURL[250] = {};	 //A Place to put the WebHookURL
char Filter[250] = {};
char NewLogMessage[2250] = {};
char OldLogMessage[2250] = {};
char SendToWebHook[500] = {};
char SentFromWhom[250] = {};
char GarbageCollection[250] = {};
char HostURL[250] = {};
char DelayinSeconds[250] = {};

string logmessage = "";
string Version = "mtrmstnmr Ver. 00.00.02.1";
string ByWho = "By Mark Meadows";

void send_data_to_mattermost(void);
void read_config(void);
void log_function(string log_message);
void check_host_data(void);

struct curl_slist *headers = NULL;

//----------------------Main Body----------------------------------------------------

int main()
{
	read_config();//(completed)
	logmessage = "Mattermost Network Monitor has started";
	log_function(logmessage);
	logmessage ="";
	cout << "Mattermost Network Monitor has Started" << endl; // Mattermost Host Monitor

	check_host_data();




	return 0;
}
//------------------------End of Main------------------------------------------------

//-----------------------------Send Data to Mattermost-------------------------------
void send_data_to_mattermost()
{
	CURL *curl;
			   CURLcode res;
			   curl_global_init(CURL_GLOBAL_DEFAULT);
			   curl = curl_easy_init();

			   // Need to build the MatterMost URL to send the Data to the WebHook
			   //Sample Curl Command to post to mattermost
			   //curl -i -X POST -H 'Content-Type: applicati/json' -d '{"text": "This is a test of the Matermost web hook system "}' http://talk.kyin.net/hooks/6c78zsda4fy

	           sprintf(SendToWebHook,"{\"text\": \"%s  %s\"}",NewLogMessage,SentFromWhom); //We have to escape all the JSON crap!

			   curl_easy_setopt(curl, CURLOPT_URL,WebHookURL);
			   curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); //Dont Check SSL Cert.
			   curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); //Dont Check SSL Cert.
			   //curl_easy_setopt(curl, CURLOPT_POSTFIELDS, R"anydelim( {"text": "You removed public link for DFocuments "} )anydelim");
			   curl_easy_setopt(curl, CURLOPT_POSTFIELDS, SendToWebHook);
			   headers = curl_slist_append(headers, "Expect:");    //Set Header Types For JSON
			   headers = curl_slist_append(headers, "Content-Type: application/json"); //Set Header Type For JSON
			   curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

			   res = curl_easy_perform(curl);

			   if(res != CURLE_OK)
			   {
				logmessage = "Unable to Connect to Mattermost Server";
				log_function(logmessage);
				fprintf(stderr, "curl_easy_perform() failed: %s\n",
			    curl_easy_strerror(res));
		        curl_easy_cleanup(curl);
		       }

			   curl_global_cleanup();
			   strncpy(OldLogMessage,NewLogMessage,strlen(NewLogMessage));
	return;
}
//--------------------------End of Send Data to MatterMost---------------------------

//--------------------------Read Configuration File----------------------------------
void read_config()
{
	 FILE *Config_File = NULL;                        // declare config file Pointer

			 		Config_File = fopen("/etc/mtrmstnmr.conf", "r");  	// Open config file
			 		if (Config_File == NULL){
			 			logmessage = "Could not open configuration file";
			 			log_function(logmessage);
			 			printf("Could not open Config File\n");
			 			exit(1);
			 		}

			 		fscanf(Config_File,"%[^\n]\n", WebHookURL);	 //This will Read to the end of each line until a carriage return
			 		fscanf(Config_File,"%[^\n]\n", Filter);		 //This will Read to the end of each line until a carriage return
			 		fscanf(Config_File,"%[^\n]\n",SentFromWhom); //This will Read to the end of each line until a carriage return
			 		fscanf(Config_File,"%[^\n]\n",DelayinSeconds);

			 		fclose(Config_File);
			 		std::string logmessage1 = "=============================================================";
			 		log_function(logmessage1);
			 		log_function(Version);
			 		log_function(ByWho);
			 		log_function(logmessage1);
			 		logmessage1 = "";
			 		logmessage = "";
			 		logmessage1 = "The Config File Say the Log File Location = ";
			 		std::string logmessage2 = LogLocation;
			 		std::string logmessage = logmessage1 + logmessage2;
			 		log_function(logmessage);
			 		logmessage1 = "";
			 		logmessage2 = "";
			 		logmessage = "";
			 		logmessage1 = "The Config File Say the WebHookURL = ";
			 		logmessage2 = WebHookURL;
			 		logmessage = logmessage1 + logmessage2;
			 		log_function(logmessage);
			 		logmessage1 = "";
			 		logmessage2 = "";
			 		logmessage = "";
			 		logmessage1 = "The Config File Say the Filter = ";
			 		//logmessage2 = Filter;
			 		logmessage = logmessage1 + logmessage2;
			 		log_function(logmessage);
			 		logmessage1 = "";
			 		logmessage2 = "";
			 		logmessage = "";
			 		logmessage1 = "The Config File Say the SentFromWhom = ";
			 		logmessage2 = SentFromWhom;
			 		logmessage = logmessage1 + logmessage2;
			 		log_function(logmessage);
			 		logmessage1 = "";
			 		logmessage2 = "";
			 		logmessage = "";
			 		logmessage = "Config File Loaded ...";
			 		log_function(logmessage);
			 		logmessage = "";

		return;
}
//---------------------------End of Read Config File----------------------------------------------------------

/*
---------------------------Get Host to Check From Host Config File------------------------------------------
---------------------------Send a check packet to see if we can connect to host via http--------------------
---------------------------Log if we are able to connect or if we failed------------------------------------
---------------------------Go to next host and check it-----------------------------------------------------
*/

void check_host_data()
{
//----------------------------------Read host from file-----------------------------------------------------
	                   FILE *Config_File = NULL;                        // declare config file Pointer

				 		Config_File = fopen("/etc/mtrmstnmr.conf", "r");  	// Open config file
				 		if (Config_File == NULL){
				 			logmessage = "Could not open configuration file";
				 			log_function(logmessage);
				 			printf("Could not open Config File\n");
				 			exit(1);
				 		}

				 		//fscanf(Config_File,"%[^\n]\n", LogLocation);      //This will Read to the end of each line until a carriage return
				 		fscanf(Config_File,"%[^\n]\n", WebHookURL);	 //This will Read to the end of each line until a carriage return
				 		fscanf(Config_File,"%[^\n]\n", Filter);		 //This will Read to the end of each line until a carriage return
				 		fscanf(Config_File,"%[^\n]\n",SentFromWhom); //This will Read to the end of each line until a carriage return
                        fscanf(Config_File,"%[^\n]\n",DelayinSeconds);
                        fscanf(Config_File,"%[^\n]\n",GarbageCollection);
                        fscanf(Config_File,"%[^\n]\n",GarbageCollection);
                        fscanf(Config_File,"%[^\n]\n",HostURL);
                        fclose(Config_File);

                        printf("HostURL = %s\n", HostURL);
                        return;

//------------------------------------Try to connect to Host------------------------------------------------
	               CURL *curl;
				   CURLcode res;
				   curl_global_init(CURL_GLOBAL_DEFAULT);
				   curl = curl_easy_init();

				   // Need to build the MatterMost URL to send the Data to the WebHook
				   //Sample Curl Command to post to mattermost
				   //curl -i -X POST -H 'Content-Type: applicati/json' -d '{"text": "This is a test of the Matermost web hook system "}' http://talk.kyin.net/hooks/6c78zsda4fy

		           sprintf(SendToWebHook,"{\"text\": \"%s  %s\"}",NewLogMessage,SentFromWhom); //We have to escape all the JSON crap!

				   curl_easy_setopt(curl, CURLOPT_URL,WebHookURL);
				   curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); //Dont Check SSL Cert.
				   curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); //Dont Check SSL Cert.
				   //curl_easy_setopt(curl, CURLOPT_POSTFIELDS, R"anydelim( {"text": "You removed public link for DFocuments "} )anydelim");
				   curl_easy_setopt(curl, CURLOPT_POSTFIELDS, SendToWebHook);
				   headers = curl_slist_append(headers, "Expect:");    //Set Header Types For JSON
				   headers = curl_slist_append(headers, "Content-Type: application/json"); //Set Header Type For JSON
				   curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

				   res = curl_easy_perform(curl);

				   if(res != CURLE_OK)
				   {
					logmessage = "Unable to Connect to Server";
					log_function(logmessage);
					fprintf(stderr, "curl_easy_perform() failed: %s\n",
				    curl_easy_strerror(res));
			        curl_easy_cleanup(curl);
			       }

				   curl_global_cleanup();
				   strncpy(OldLogMessage,NewLogMessage,strlen(NewLogMessage));
//------------------------------------------End of Try to connect to host---------------------------------


				   return;
}
//----------------------------End of Read Host File and check host----------------------------------------

//-----------------------------Safe logg message to Log File----------------------------------------------
void log_function(string log_message)
{

	struct timespec ts;
		    timespec_get(&ts, TIME_UTC);
		    char buff[100];
		    strftime(buff, sizeof buff, "%D %T", gmtime(&ts.tv_sec));

			 ofstream file;
			 file.open ("/var/log/mtrmstnmr.log",std::ios_base::app);

			char MyTime[27];
			sprintf(MyTime," %s.%09ld " , buff,ts.tv_nsec); //Format and apply data

			file<<MyTime;
			file<<log_message;
			file<<"\n";
			file.close();
}
//----------------------End of Log Function------------------------------------------
