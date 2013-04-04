#undef UNICODE
#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
//#include <windows.h>               // requires "define" statement above
//#include <winsock2.h>
//#include <C:\Program Files\Microsoft SDKs\Windows\v7.0A\Include\Ws2tcpip.h>
#include <ws2tcpip.h>

//#include <c:\Program Files\Microsoft SDKs\Windows\v7.0A\Include\Winhttp.h>        // - really needed!!!

#include <stdio.h>
// #include <ctype.h>             //????? can live without it
// include <netdb.h>
#include <stdlib.h>            //????? can live without it
// #include <string.h>
// #include <arpa/inet.h>
// #include <sys/types.h>         //????? can live without it
// #include <sys/socket.h>
// #include <netinet/in.h>
#include <malloc.h>

#pragma comment(lib, "Ws2_32.lib")
// #pragma comment(lib, "Mswsock.lib")
// #pragma comment(lib, "AdvApi32.lib")

int main(int argc, char **argv)
{
	WSADATA wsaData;
	int iRes, iS, iError,iStrToSendLen, iBytesReceived,iTotalBytesReceived;
	//struct addrinfo *hints;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL, *ptr = NULL;
	struct sockaddr current_socket_addr;
	struct sockaddr_in conServAddr;
	//char strToSend[17] = "GET / HTTP/1.0\n\n";              // this one works with basic html page provided with Apache 
	//char strToSend[33] = "POST http://127.0.0.1:80/cgi-bin/getit.cgi HTTP/1.0\n\n";
	char strToSend[128] = "<FORM action=\"http://10.18.52.119/cgi-bin/getit_more.cgi?\" method=\"post\"></FORM>\n\n";
	char recvBuffer[32];
	//struct addrinfo hints;
	char parameters[16] = "Just my 2 cents";
	char content_header[128];
	char postbuf[1024];
	u_long iMode;
	//char boundaryline[16]="--------76865412";
	FILE * thesource;
	size_t size_read;
	char  buf_read[64];  // previously: 64

	
	printf ("This is a native C program.\n");
	
	iRes = WSAStartup(MAKEWORD(2,2),&wsaData);
	if (iRes != 0) 
	{
		printf("WSAStartup Failed.\n");
	}
	else
	{
		printf("\n");
		printf("WSA Version: %ld\n", wsaData.wVersion);
		printf("WSA High Version: %ld\n", wsaData.wHighVersion);
		printf("WSA Description: %s\n", wsaData.szDescription);
		printf("WSA System Status: %s\n", wsaData.szSystemStatus);
		printf("WSA Max Sockets: %d\n", wsaData.iMaxSockets);
		printf("WSA Max Udp Dg: %d\n", wsaData.iMaxUdpDg);
		//printf("WSA Vendor Info: %s\n", wsaData.lpVendorInfo);
		printf("\n");
	}
	
	ConnectSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);    //AF_UNSPEC
	if (ConnectSocket == INVALID_SOCKET)
	{
		printf(" Invalid socket: %ld\n", WSAGetLastError());
		
	}
	//fcntl(ConnectSocket,F_SETFL,O_NONBLOCK);
	
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
	
	iStrToSendLen = strlen(strToSend);
	iRes = send(ConnectSocket,strToSend,iStrToSendLen,0);
	if (iRes != iStrToSendLen) 
	{
		printf(" Send Error: result = %d, expected = %d\n", iRes, iStrToSendLen);
	}	
	
	iBytesReceived = 0;
	iTotalBytesReceived = 0;
	memset(recvBuffer,'\0',sizeof(recvBuffer));
	printf("****Received: \n");
	do 
	{
		iBytesReceived = recv(ConnectSocket, recvBuffer,31,0);
		if (iBytesReceived < 0) 
		{
			printf(" Recv Error, %d\n", iBytesReceived);
		}
		printf("%s",recvBuffer);
		memset(recvBuffer,'\0',sizeof(recvBuffer));
		iTotalBytesReceived += iBytesReceived;
	} while (iBytesReceived > 0);
	
	printf("\n Total of Bytes Received: %d\n", iTotalBytesReceived);
	//iS = sizeof(struct addrinfo);
	//hints = malloc(sizeof(struct addrinfo));
	printf("\n");
	
	////////////////////////////////////////////////////////////
	//
	// This part of code deals with the fact that 
	// there are no indication of connection to the server
	// after first and only "send-receive" cycle,
	// although the program does not report any problem 
	// with the seconds attemt to send and receive;
	// it just returns nothing
	// Should status of the socket/connection be verified 
	// and changed before attempting second "send-receive"?
	//
	
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
	
	memset(postbuf,'\0', sizeof(postbuf));
	//sprintf(postbuf,"%s","GET //cgi-bin//getit.cgi HTTP/1.0\r\n");
	sprintf(postbuf,"%s", "<FORM action=\"http://sqatest1/cgi-bin/getit.cgi?\" method=\"post\"></FORM>\n\n");
	// sprintf(postbuf,"%s%s%d%s",postbuf,"Content-Length: ",strlen(parameters),"\r\n");
	// sprintf(postbuf,"%s%s",postbuf,"Content-Type: application/x-www-form-urlencoded\r\n\r\n\r\n");
	// sprintf(postbuf,"%s%s%s",postbuf,parameters,"\r\n");

	printf("\n About To Be Send: \n %s \n",postbuf);
	
	iStrToSendLen = strlen(postbuf);
	iRes = send(ConnectSocket,postbuf,iStrToSendLen,0);
	//iRes = send(ConnectSocket,strToSend,iStrToSendLen,0);
	if (iRes != iStrToSendLen) 
	{
		printf(" Send Postbuf Error: result = %d, expected = %d\n", iRes, iStrToSendLen);
	}
	
	///////////////////////////////////////////////
	iBytesReceived = 0;
	iTotalBytesReceived = 0;
	memset(recvBuffer,'\0',sizeof(recvBuffer));
	printf("****Second Received: \n");
	do 
	{
		iBytesReceived = recv(ConnectSocket, recvBuffer,31,0);
		if (iBytesReceived < 0) 
		{
			printf(" Seconds Recv Error, %d\n", iBytesReceived);
		}
		printf("%s",recvBuffer);
		memset(recvBuffer,'\0',sizeof(recvBuffer));
		iTotalBytesReceived += iBytesReceived;
	} while (iBytesReceived > 0);
	
	printf("\n Second Total of Bytes Received: %d\n", iTotalBytesReceived);
	
	printf("\n");
	///////////////////////////////////////////////
	
	////////// addition of 02 20 2013 /////////////
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

	//// !!!! VARIANT 1: PROVEN 02/25/2013
	// sprintf(postbuf,"%s","POST /cgi-bin/getit_even_more.cgi?m=50&name=Eugene ");
	// sprintf(postbuf,"%s%s",postbuf,"Host: sqatest1\n");
	// sprintf(postbuf,"%s%s",postbuf,"Accept: text/html\n");
	// sprintf(postbuf,"%s%s",postbuf,"Content-type: application/x-www-form-urlencoded\n");
	// sprintf(postbuf,"%s%s%d%s",postbuf,"Content-Length: ",strlen("value_one=my&value_two=2&value_three=cents"),"\n");
	// sprintf(postbuf,"%s%d",postbuf,"Keep-Alive: 300\n");               // optional ?
	// sprintf(postbuf,"%s%s",postbuf,"Connection: keep-alive\n\n");      // optional ? 
	// // the line below places "payload" into the message; 
	// // watch for double new line "\n\n" characters in 
	// // whatever comes right before that line
	// sprintf(postbuf,"%s%s",postbuf,"value_one=my&value_two=2&value_three=cents\n\n");
	//// 
	//// end of VARIANT 1
	///////////////////////////////////////
	//// !!!! VARIANT 2
	sprintf(postbuf,"%s","POST 	/cgi-bin/getit_even_more.cgi?m=57&name=Eugene ");
	sprintf(postbuf,"%s%s",postbuf,"Host: sqatest1\n");
	sprintf(postbuf,"%s%s",postbuf,"Accept: text/plain, text/html;\n");
	//sprintf(postbuf,"%s%s",postbuf,"Content-type: application/x-www-form-urlencoded\n");
	sprintf(postbuf,"%s%s",postbuf,"Content-type: multipart/form-data, boundary=\"AaB03\"\n");
	
	////!!!!!sprintf(postbuf,"%s%s%d%s",postbuf,"Content-Length: ",strlen("...This file is short\n\n"),"\n\n");
	sprintf(postbuf,"%s%s%d%s",postbuf,"Content-Length: ",64,"\n\n");   //previously:52; 63
	thesource = fopen("c:\\eugene\\cpp\\EXAMPLE FOR UPLOAD.TXT","r");
	size_read = fread(buf_read,1,64,thesource);   // previously: 51; 63
	fclose(thesource);
	printf("\n %s%s \n ","Just read: ", buf_read);
	//buf_read[64]='\n';   // previously: 52
	//buf_read[51]='\n';
	
	//sprintf(postbuf,"%s%s%s",postbuf,"--AaB03","\n");
	//sprintf(postbuf,"%s%s",postbuf, "content-disposition: form-data; name=\"myfile\"; filename=\"EXAMPLE FOR UPLOAD.txt\"\n\n");
	//sprintf(postbuf,"%s%s",postbuf,"Content-Type:text/plain\n\n");
	// sprintf(postbuf,"%s%s%d%s",postbuf,"Keep-Alive: ",180,"\n");               // optional ?
	// sprintf(postbuf,"%s%s",postbuf,"Connection: keep-alive\n");      // optional ? 
	// sprintf(postbuf,"%s%s%s",postbuf,"----------------","\n\n");
	// sprintf(postbuf,"%s%s%s%s",postbuf, "Content-Disposition: form=data; name=\"file\"; filename=\"C:\\Eugene\\cpp\\EXAMPLE FOR UPLOAD.txt","\r\n", "Content-Type: text/plain\r\n\r\n");
	// sprintf(postbuf,"%s%s",postbuf,"----------------");
	// the line below places "payload" into the message; 
	// watch for double new line "\n\n" characters in 
	// whatever comes right before that line
	
	////!!!!sprintf(postbuf,"%s%s",postbuf,"...This file is short\n\n");
	sprintf(postbuf,"%s%s",postbuf,buf_read);
	
	//sprintf(postbuf,"%s%s%s",postbuf,"--AaB03--","\n\n");
	//// 
	//// end of VARIANT 2
	///////////////////////////////////////
	
	//////////////////////////
	//// VARIANT 3
	// // sprintf(postbuf,"%s","<TITLE>File Upload</TITLE><FORM method=\"post\" action=\"SQATEST1/cgi-bin/getit_even_more.cgi?m=57&name=Eugene\" enctype=\"multipart/form-data\">\n");
	// // sprintf(postbuf,"%s%s",postbuf,"<input type=\"file\" name=\"C:\\Eugene\\cpp\\EXAMPLE FOR UPLOAD.TXT\"><br/>\n");
	// // sprintf(postbuf,"%s%s",postbuf,"<input type=\"submit\" name=\"Submit\" value=\"Upload Files\"/>\n");
	// // sprintf(postbuf,"%s%s",postbuf, "</FORM>\n\n");
	////
	//// END OF VARIANT 3
	////////////////////////
	
	printf("\n About To Be Send: \n %s \n",postbuf);
	
	iStrToSendLen = strlen(postbuf);
	iRes = send(ConnectSocket,postbuf,iStrToSendLen,0);
	//iRes = send(ConnectSocket,strToSend,iStrToSendLen,0);
	if (iRes != iStrToSendLen) 
	{
		printf(" Third Send Postbuf Error: result = %d, expected = %d\n", iRes, iStrToSendLen);
	}
	
	///////////////////////////////////////////////
	iBytesReceived = 0;
	iTotalBytesReceived = 0;
	memset(recvBuffer,'\0',sizeof(recvBuffer));
	printf("****Third Received: \n");
	do 
	{
		iBytesReceived = recv(ConnectSocket, recvBuffer,31,0);
		if (iBytesReceived < 0) 
		{
			printf(" Third Recv Error, %d\n", iBytesReceived);
		}
		printf("%s",recvBuffer);
		memset(recvBuffer,'\0',sizeof(recvBuffer));
		iTotalBytesReceived += iBytesReceived;
	} while (iBytesReceived > 0);
	
	printf("\n Third Total of Bytes Received: %d\n", iTotalBytesReceived);
	
	printf("\n");
	
	//// end of addition of 02 20 2013
	closesocket(ConnectSocket);
	WSACleanup();
	
	return 0;
	
}