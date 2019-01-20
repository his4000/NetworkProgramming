#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#define RCVBUFSIZE 32  //size of receive buffer
#define IP_SIZE 15  //size of IP address

#define CHECK_SIG "hello"  //send check signal
#define CHECK_RCV_SIG "hi"  //receive check signal
#define EXIT_SIG "quit"  //exit signal
#define FT_SIG "FT"  //File Transfer start signal

void DieWithError(char *errorMessage);  //Error Handling Function
void FileSend(int servSock);  //File Send

int main(int argc, char* argv[]){
	int sock;  //Socket descriptor
	struct sockaddr_in ServAddr;  //Server address
	unsigned short ServPort;  //Server port
	char servIP[IP_SIZE];  //Server IP address
	char msgString[RCVBUFSIZE];  //message string
	char buffer[RCVBUFSIZE];  //string buffer
	int bytesRcvd;  //size of Received Bytes
	char check_sig[RCVBUFSIZE];  //check signal
	
	strncpy(check_sig, CHECK_SIG, RCVBUFSIZE);

	/* Test for correct number of arguments */
	if(argc != 3){
		fprintf(stderr, "Usage: %s <Server IP> <Server Port>\n",argv[0]);
		exit(1);
	}
	strcpy(servIP, argv[1]);  //First arg: server IP address
	
	ServPort = atoi(argv[2]);  //Use given port

	/*Create a reliable, stream socket using TCP*/
	if((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		DieWithError("socket() failed");

	/*Construct the server address structure*/
	memset(&ServAddr, 0, sizeof(ServAddr));  //zero out structure
	ServAddr.sin_family = AF_INET;  //internet address family
	ServAddr.sin_addr.s_addr = inet_addr(servIP);  //Server IP address
	ServAddr.sin_port = htons(ServPort);  //Server Port

	/*Establish the connection to the echo server*/
	if(connect(sock, (struct sockaddr*)&ServAddr, sizeof(ServAddr)) < 0)
		DieWithError("connect() failed");
	if(send(sock, check_sig, RCVBUFSIZE, 0) != RCVBUFSIZE)
		DieWithError("ACK send() failed in TCPEchoClient.c line 54");
	if(recv(sock, check_sig, RCVBUFSIZE, 0) < 0)
		DieWithError("ACK recv() failed in TCPEchoClient.c line 56");

	printf("\n");
	close(sock);
	exit(0);
}
