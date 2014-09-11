
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>

#include <iostream>
#include <cstring>
#include <string>

using namespace std;

class FileRouter {

	public:
		
		string strRemoteHost, strRemoteAddr, strQueryString;

		// constructor
		FileRouter()
		{
			char * envparam;
			
			envparam = getenv("REMOTE_HOST");
			if (envparam != NULL)
			{
				strRemoteHost = envparam;
			}

			envparam = getenv("REMOTE_ADDR");
			if (envparam != NULL)
			{
				strRemoteAddr = envparam;
			}
			
			envparam = getenv("QUERY_STRING");
			if (envparam != NULL)
			{
				strQueryString = envparam;
			}
			
		};
		
		// a method
		string unique_filename_suffix()
		{
			unsigned dot_located_at = 0;
			size_t start_at = 0, end_at = 0;
			string strDuplicate = strRemoteAddr;
			
			string strParamName = "startedAt=";
			string strNextParam = "&EndOfFile";
			
			string time_stamp_retrieved;
			
			//// to implement preventive steps
			//// against iteration over empty strRemoteAddr
			while (dot_located_at != string::npos)
			{
				dot_located_at = strDuplicate.find('.', 0);
				
				if (dot_located_at != string::npos)
				{
					strDuplicate.erase(dot_located_at,1);
				}

			}
						
			// retrieval of  timestamp from query string
			start_at = strQueryString.find(strParamName, 0);

			if (start_at != string::npos)
			{
				start_at += strParamName.length();
			}
			
			end_at = strQueryString.find(strNextParam);
			
			if (end_at != string::npos)
			{
				time_stamp_retrieved = strQueryString.substr(start_at,(end_at - start_at));
			}			
			
			return (strDuplicate + time_stamp_retrieved);
		}
		
		// destructor
		~FileRouter(){};

};


int main(int argv, char** argc)
{
	char s[512];  
	FILE * mylog, * local_copy;
	char logline[256];
	time_t current_time;
	char c_time_string[32];
	int i;
	
	printf("QUERY_STRING  : %s\n\n",getenv("QUERY_STRING"));
	printf("CONTENT TYPE  : %s\n\n",getenv("CONTENT_TYPE"));
	printf("CONTENT_LENGTH: %s\n\n",getenv("CONTENT_LENGTH"));
	printf("HOSTNAME      : %s\n\n",getenv("HOSTNAME"));
	printf("REQUEST METHOD: %s\n\n",getenv("REQUEST_METHOD"));
	printf("REMOTE_ADDR   : %s\n\n",getenv("REMOTE_ADDR"));
	printf("REMOTE_HOST   : %s\n\n",getenv("REMOTE_HOST"));
	printf("SCRIPT_NAME   : %s\n\n",getenv("SCRIPT_NAME"));
	printf("QUERY_STRING  : %s\n\n",getenv("QUERY_STRING"));
	
	FileRouter fr;

	cout << "*****\n" << fr.strRemoteHost << ", " << fr.strRemoteAddr << ", "<< fr.strQueryString<< "****\n" <<endl;
	
	memset(s,'\0',sizeof(s));
	fgets(s, sizeof(s), stdin);
	
	// output for debugging: goes directly through the socket
	// to the client's end
	printf("That's what I got: (size of s is %d) %s\n",  sizeof(s),s);
	
	// recovering the "newline" characters encoded 
	// by the client as an unused ASCII character (code decimal 17)
	for (i = 0; i < sizeof(s); i++)
	{
		if (s[i] == 17) 
		{			                                    
			s[i] = '\n';
		}
	}
	printf("That's what I got after: %s\n", s);
	
	// creating a unique name for the local copy via concatenation
	string strLocalCopyName = "c:\\eugene\\cpp\\Local_Copy_" + fr.unique_filename_suffix() + ".txt";
	
	// writing to the file
	local_copy = fopen(strLocalCopyName.c_str(),"a+");
	fprintf(local_copy,"%s",s);
	fclose(local_copy);
	
	
	memset(c_time_string,'\0',sizeof(c_time_string));
	current_time = time(NULL);
	
	// output for debugging: goes directly through the socket
	// to the client's end
	printf("\n %s \n\n", ctime(&current_time));

	memcpy(c_time_string,ctime(&current_time),24);
	
	memset(logline,'\0',sizeof(logline));
	sprintf(logline,"%s%s%s\n",c_time_string," just a test ",s);
	
	// writing to log file for debugging and diagnostics
	mylog = fopen("c:\\eugene\\cpp\\mylog.txt","a+");
	fprintf(mylog,logline);
	fprintf(mylog,"\n");
	
	
	fclose(mylog);
	
	return 0;
	
}

