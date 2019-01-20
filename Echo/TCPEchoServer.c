#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#define IP_SIZE 15  //IP Address Size
#define RCVBUFSIZE 32  //Size of receive buffer
#define MAXPENDING 5  //Maximum outstanding connection requests

void DieWithError(char* errorMessage);  //Error handling function
void HandleTCPClient(int clntSocket);  //TCP client handling function

int main(int argc, char* argv[]){
	int servSock;  //Socket descriptor for server
	int clntSock;  //Socket descriptor for client
	struct sockaddr_in echoServAddr;  //Local address
	struct sockaddr_in echoClntAddr;  //Client address
	unsigned short echoServPort;  //Server port
	unsigned int clntLen;  //Length of client address data structure
	char clntIP[IP_SIZE];  //Client IP Address 
	int checkMsgSize;  //Initial check message size
	char checkEchoBuffer[RCVBUFSIZE];  //initial check buffer
	char checkSign[RCVBUFSIZE] = "hi";  //Check Sign

	printf("client ip : ");
	scanf("%s",clntIP);
	printf("port : ");
	scanf("%hu",&echoServPort);

	/*Create socket for incoming connections*/
	if((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		DieWithError("socket() failed");

	/*Construct local address structure*/
	memset(&echoServAddr, 0, sizeof(echoServAddr));  //Zero out structure
	echoServAddr.sin_family = AF_INET;  //Internet address family
	echoServAddr.sin_addr.s_addr = inet_addr(clntIP);  //Client IP address
	echoServAddr.sin_port = htons(echoServPort);  //Local port

	/*Bind to the local address*/
	if(bind(servSock, (struct sockaddr*)&echoServAddr, sizeof(echoServAddr)) < 0)
		DieWithError("bind() failed");

	/*Mark the socket so it will listen for incoming connections*/
	if(listen(servSock, MAXPENDING) < 0)
		DieWithError("listen() failed");

	for(;;)  //Run forever
	{
		clntLen = sizeof(echoClntAddr);  //Set the size of the in-out parameter

		if((clntSock = accept(servSock, (struct sockaddr*)&echoClntAddr, &clntLen)) < 0)
			DieWithError("accept() failed");
			
		if(checkMsgSize = recv(clntSock, checkEchoBuffer, RCVBUFSIZE, 0) < 0)  //Initial Check Receive
			DieWithError("check recv() failed");
		else if(strcmp("hello",checkEchoBuffer) == 0){  //Check Received
			printf("msg<- %s\n",checkEchoBuffer);  //Check message print
			if(send(clntSock, checkSign, RCVBUFSIZE, 0) != sizeof(checkSign))  //Check message send
				DieWithError("check send() failed");
			else{
				printf("msg-> %s\n",checkSign);  //check message send print
				HandleTCPClient(clntSock);
			}
		}
	}

}
