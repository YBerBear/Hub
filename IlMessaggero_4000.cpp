#undef UNICODE
#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>

#define _WIN32_WINNT 0x0500
#include <windows.h>               // requires "define" statement above
#include <iostream>
//#include <C:\Program Files\Microsoft SDKs\Windows\v7.0A\Include\Ws2tcpip.h>
#include <ws2tcpip.h>

//#include <c:\Program Files\Microsoft SDKs\Windows\v7.0A\Include\Winhttp.h>        // - really needed!!!

#include <stdio.h>
#include <stdlib.h>            //????? can live without it
#include <malloc.h>
#include <ctime>

#pragma comment(lib, "Ws2_32.lib")

#define PAYLOADBUFSIZE 512

/* void pause(int iDur)
{
	int temp;
	
	temp = time(NULL) + iDur;
	
	while(time(NULL) < temp);
} */

int main(int argc, char **argv)
{
	WSADATA wsaData;
	int iRes, iS, iError,iStrToSendLen, iBytesReceived,iTotalBytesReceived,iCharCount,  iSentBufferCount;
	
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL, *ptr = NULL;
	struct sockaddr current_socket_addr;
	struct sockaddr_in conServAddr;
	//char strToSend[17] = "GET / HTTP/1.0\n\n";              // this one works with basic html page provided with Apache 
	//char strToSend[33] = "POST http://127.0.0.1:80/cgi-bin/getit.cgi HTTP/1.0\n\n";
	//char strToSend[128] = "<FORM action=\"http://10.18.52.119/cgi-bin/getit_more.cgi?\" method=\"post\"></FORM>\n\n";
	char recvBuffer[32];
	char timestamp[16];
	char postbuf[PAYLOADBUFSIZE * 2];
	u_long iMode;
	
	FILE * thesource;
	size_t size_read;
	char  buf_read[PAYLOADBUFSIZE];  // previously: 64
	char current_char;

	FreeConsole();

	// for debugging mostly
	// printf ("This is a native C program.\n");
	
	iRes = WSAStartup(MAKEWORD(2,2),&wsaData);
	if (iRes != 0) 
	{
		printf("WSAStartup Failed.\n");
	}
	else
	{
		// for debugging mostly
		// printf("\n");
		// printf("WSA Version: %ld\n", wsaData.wVersion);
		// printf("WSA High Version: %ld\n", wsaData.wHighVersion);
		// printf("WSA Description: %s\n", wsaData.szDescription);
		// printf("WSA System Status: %s\n", wsaData.szSystemStatus);
		// printf("WSA Max Sockets: %d\n", wsaData.iMaxSockets);
		// printf("WSA Max Udp Dg: %d\n", wsaData.iMaxUdpDg);
		// printf("\n");
	}
	

	closesocket(ConnectSocket);
	ConnectSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);    //AF_UNSPEC
	if (ConnectSocket == INVALID_SOCKET)
	{
		printf(" Invalid third socket: %ld\n", WSAGetLastError());
		
	}
	
	memset(&conServAddr, 0, sizeof(conServAddr));
	conServAddr.sin_family      = AF_INET;
	conServAddr.sin_addr.s_addr = inet_addr("10.18.52.119");
	conServAddr.sin_port        = htons(80);
	
	iRes = connect(ConnectSocket, (struct sockaddr *)&conServAddr,sizeof(conServAddr));
	if (iRes == SOCKET_ERROR) 
	{
			printf(" Third Socket Error, addr len: %d\n",strlen("10.18.52.119:80"));
			iError = WSAGetLastError();
			printf(" The error: %d\n", iError);
			if (iError == WSAEFAULT)
			{
				printf(" This is WSAEFAULT, %d\n", iError);
			}
	}
	// end of re-establishing the socket connection with the server
	/////////////////////////////////////////////////////////////////////////
	
	memset(postbuf,'\0', sizeof(postbuf));
	memset(buf_read,'\0',sizeof(buf_read));

	// thesource = fopen("c:\\eugene\\cpp\\EXAMPLE FOR UPLOAD.TXT","r");
	//thesource = fopen("c:\\temp\\prelim\\carrier10381.TXT","r");
	//thesource = fopen("c:\\Eugene\\testsofgnuplotandoctave\\logs\\Cirrus_log.txt","r");
	
	// thesource = fopen("c:\\ProgramData\\Carl Zeiss Meditec\\Cirrus background processing service\\Logs\\Log.txt","r");
	thesource = fopen("c:\\Documents and settings\\All Users\\Application Data\\Carl Zeiss Meditec\\Cirrus background processing service\\Logs\\Log.txt","r");
	
	iSentBufferCount = 1;
	memset(timestamp,'\0',sizeof(timestamp));
	sprintf(timestamp,"%d",time(NULL));
	
	iCharCount = 0;
	
	do
	{
		while (iCharCount < (PAYLOADBUFSIZE - 1)) 
		{
			current_char = fgetc(thesource);
			if (current_char == EOF)
			{
				break;
			}
			
			if (current_char == '\n') 
			{
				buf_read[iCharCount]=17;          // Decimal ASCII code for DC1
				                
				//break;
			}
			else
			{
				buf_read[iCharCount]=current_char;
			}
			iCharCount++;
		}
		///////////////////////////
		//sprintf(postbuf,"%s","POST 	/cgi-bin/conveyor.cgi?m=57&name=Eugene ");
		sprintf(postbuf,"%s%d%s%s","POST 	/cgi-bin/IlCorriere.cgi?idxBufferSent=",iSentBufferCount,"&startedAt=",timestamp);
		if (current_char == EOF)
		{
			strcat(postbuf,"&EndOfFile=TRUE ");
			
		}
		else
		{
			strcat(postbuf,"&EndOfFile=FALSE ");
		}
		sprintf(postbuf,"%s%s",postbuf,"Host: sqatest1\n");
		sprintf(postbuf,"%s%s",postbuf,"Accept: text/plain, text/html;\n");
		sprintf(postbuf,"%s%s",postbuf,"Content-type: multipart/form-data, boundary=\"AaB03\"\n");
		sprintf(postbuf,"%s%s%d%s",postbuf,"Content-Length: ",iCharCount,"\n\n");
		sprintf(postbuf,"%s%s",postbuf,buf_read);
		
		// for debugging mostly
		// printf("\n About To Be Send: \n %s \n",postbuf);  
		
		/////
		closesocket(ConnectSocket);
		ConnectSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);    //AF_UNSPEC
		if (ConnectSocket == INVALID_SOCKET)
		{
			printf(" Invalid second socket: %ld\n", WSAGetLastError());
		
		}
	
		memset(&conServAddr, 0, sizeof(conServAddr));
		conServAddr.sin_family      = AF_INET;
		conServAddr.sin_addr.s_addr = inet_addr("10.18.52.119");
		conServAddr.sin_port        = htons(80);
	
		iRes = connect(ConnectSocket, (struct sockaddr *)&conServAddr,sizeof(conServAddr));
		if (iRes == SOCKET_ERROR) 
		{
			printf(" Socket Error, addr len: %d\n",strlen("10.18.52.119:80"));
			iError = WSAGetLastError();
			printf(" The error: %d\n", iError);
			if (iError == WSAEFAULT)
			{
				printf(" This is WSAEFAULT, %d\n", iError);
			}
		}
	// end of re-establishing the socket connection with the server
	/////////////////////////////////////////////////////////////////////////
		/////
		
		iStrToSendLen = strlen(postbuf);
		iRes = send(ConnectSocket,postbuf,iStrToSendLen,0);
		if (iRes != iStrToSendLen) 
		{
			printf(" Third Send Postbuf Error: result = %d, expected = %d\n", iRes, iStrToSendLen);
		}
	
		// preparing and dealing with response
		iBytesReceived = 0;
		iTotalBytesReceived = 0;
		memset(recvBuffer,'\0',sizeof(recvBuffer));
		
		// for debugging mostly
		//printf("****Third Received: \n");
		do 
		{
			iBytesReceived = recv(ConnectSocket, recvBuffer,31,0);
			if (iBytesReceived < 0) 
			{
				printf(" Third Recv Error, %d\n", iBytesReceived);
			}
			
			// for debugging mostly
			// printf("%s",recvBuffer);
			
			memset(recvBuffer,'\0',sizeof(recvBuffer));
			iTotalBytesReceived += iBytesReceived;
		} while (iBytesReceived > 0);
	
		// for debugging mostly
		// printf("\n Third Total of Bytes Received: %d\n", iTotalBytesReceived);
		// printf("\n");
	
		closesocket(ConnectSocket);
		//pause(1);
	
		
		iCharCount = 0;
		memset(postbuf,'\0', sizeof(postbuf));
		memset(buf_read,'\0',sizeof(buf_read));
		
		iSentBufferCount++;
		
	}while(current_char != EOF);
	
	fclose(thesource);
	
	WSACleanup();
	
	return 0;
	
}

