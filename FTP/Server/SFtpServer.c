#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#define MAXPENDING 5  //Maximum outstanding connection requests

#define RCVBUFSIZE 32  //size of receiver buffer
#define IP_SIZE 15  //size of IP address

#define CHECK_SIG "hi"  //send check signal
#define CHECK_RCV_SIG "hello"  //receive check signal
#define EXIT_SIG "quit"  //exit signal

void DieWithError(char* errorMessage);  //Error Handling Function
void HandleTCPClient(int clntSocket);  //TCP client handling function
void FileRecv(int); //File Receiver

int main(int argc, char* argv[]){
	int servSock;  //Server socket descriptor
	int clntSock;  //Client socket descriptor
	struct sockaddr_in ServAddr;  //Local address
	struct sockaddr_in ClntAddr;  //Client address
	char clntIP[IP_SIZE];  //Client IP address
	unsigned short ServPort;  //Server port
	unsigned int clntLen;  //Length of client address data structure
	int recvMsgSize;
	char buffer[RCVBUFSIZE];
	char check_sig[RCVBUFSIZE];

	strncpy(check_sig,CHECK_SIG,RCVBUFSIZE);

	if(argc != 2){
		fprintf(stderr, "Usage: %s <Server Port>\n",argv[0]);
		exit(1);
	}

	ServPort = atoi(argv[1]);  //First arg: local port

	/*Create socket for incoming connections*/
	if((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		DieWithError("socket() failed");

	/*Construct local address structure*/
	memset(&ServAddr, 0, sizeof(ServAddr));  //zero out structure
	ServAddr.sin_family = AF_INET;  //internet address family
	ServAddr.sin_addr.s_addr = htonl(INADDR_ANY);  //client ip address
	ServAddr.sin_port = htons(ServPort);  //Local port

	/*Bind to the local address*/
	if(bind(servSock, (struct sockaddr*)&ServAddr, sizeof(ServAddr)) < 0)
		DieWithError("bind() failed");

	/*Mark the socket so it will listen for incoming connections*/
	if(listen(servSock, MAXPENDING) < 0)
		DieWithError("listen() failed");

	/*Run Forever*/
	while(1){
		/*Set the size of the in-out parameter*/
		clntLen = sizeof(ClntAddr);

		/*Wait for a client to connect*/
		if((clntSock = accept(servSock, (struct sockaddr*)&ClntAddr, &clntLen)) < 0)
			DieWithError("accept() failed");
	
		if(recv(clntSock, check_sig, RCVBUFSIZE, 0) < 0)
			DieWithError("ACK recv() failed in TCPEchoServer.c line 70");
		if(send(clntSock, check_sig, RCVBUFSIZE, 0) != RCVBUFSIZE)
			DieWithError("ACK send() failed in TCPEchoServer.c line 72");
		FileRecv(clntSock);
	}
}
