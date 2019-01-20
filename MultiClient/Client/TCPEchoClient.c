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

int main(void){
	int sock;  //Socket descriptor
	struct sockaddr_in ServAddr;  //Server address
	unsigned short ServPort;  //Server port
	char servIP[IP_SIZE];  //Server IP address
	char msgString[RCVBUFSIZE];  //message string
	char buffer[RCVBUFSIZE];  //string buffer
	int bytesRcvd;  //size of Received Bytes
	char check_sig[RCVBUFSIZE];  //check signal
	
	strncpy(check_sig, CHECK_SIG, RCVBUFSIZE);

	printf("=============== Manual =================\n\n");
	printf("   FT를 입력하면 File Transfer 수행\n");
	printf("   quit을 입력하면 종료\n\n");
	printf("========================================\n");
	printf("Server IP : ");
	scanf("%s",servIP);  //input Server IP
	printf("Server Port : ");
	scanf("%hu",&ServPort);  //input Server Port

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
		DieWithError("check send() failed");
	else
		printf("msg-> %s\n",CHECK_SIG);
	if((bytesRcvd = recv(sock, buffer, RCVBUFSIZE, 0)) < 0)
		DieWithError("check recv() failed");

	if(strcmp(buffer, CHECK_RCV_SIG) == 0)
		printf("msg<- %s\n",CHECK_RCV_SIG);
	else
		DieWithError("check_signal is different");
		

	while(getchar()!='\n');  //flush stdin
	/*Loop of Send*/
	while(1){
		printf("msg-> ");
		gets(msgString);  //get msgString
		/*Exit signal input*/
		if(strcmp(msgString,EXIT_SIG) == 0){
			if(send(sock, msgString, RCVBUFSIZE, 0) != RCVBUFSIZE)
				DieWithError("EXIT_SIG send() failed");
			if((bytesRcvd = recv(sock, buffer, RCVBUFSIZE, 0)) < 0)
				DieWithError("EXIT_SIG signal recv() failed");
			break;			
		}
		/*File transfer signal input*/
		if(strcmp(msgString,FT_SIG) == 0){
			if(send(sock, msgString, RCVBUFSIZE, 0) != RCVBUFSIZE)
				DieWithError("File transfer signal send() failed in TCPEchoClient.c line 84");
			if((bytesRcvd = recv(sock, buffer, RCVBUFSIZE, 0)) < 0)
				DieWithError("File transfer signal recv() failed in TCPEchoClient.c line 86");
			FileSend(sock);
		}
		
		/*Message sending mode*/
		else{
			if(send(sock, msgString, RCVBUFSIZE, 0) != RCVBUFSIZE)
				DieWithError("send() sent a different number of bytes than expected");
			
			if((bytesRcvd = recv(sock, buffer, RCVBUFSIZE, 0)) < 0)
				DieWithError("recv() failed or connection closed prematurely");
			if(strcmp(buffer, FT_SIG) != 0)
				printf("msg<- %s\n",buffer);
		}
	}
	printf("\n");
	close(sock);
	exit(0);
}
