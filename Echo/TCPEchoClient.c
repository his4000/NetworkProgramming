#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#define IP_SIZE 15  //IP Address Size
#define RCVBUFSIZE 32  //Size of receiver buffer

void DieWithError(char *errorMessage);  //Error handling function

int main(int argc, char *argv[]){
	int sock;  //Socket descriptor
	struct sockaddr_in echoServAddr;  //Echo server address
	unsigned short echoServPort;  //Echo server port
	char servIP[IP_SIZE];  //Server IP address
	char echoString[RCVBUFSIZE];  //String to send to echo server
	char echoBuffer[RCVBUFSIZE];  //Buffer for echo string
	unsigned int echoStringLen;  //Length of string to echo
	int bytesRcvd;  //Bytes read in single recv()
	int totalBytesRcvd;  //total bytes read
	char checkEchoBuffer[RCVBUFSIZE];  //initial check buffer
	char checkSign[RCVBUFSIZE] = "hello";  //Check Sign

	printf("server ip : ");
	scanf("%s",servIP);
	printf("port : ");
	scanf("%hu",&echoServPort);

	/*Create a reliable, stream socket using TCP*/
	if((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		DieWithError("socket() failed");

	/*Construct the server address structure*/
	memset(&echoServAddr, 0, sizeof(echoServAddr));  //Zero out structure
	echoServAddr.sin_family = AF_INET;  //Internet address family
	echoServAddr.sin_addr.s_addr = inet_addr(servIP);  //Server IP address
	echoServAddr.sin_port = htons(echoServPort);  //Server Port

	/*Establish the connection to the echo server*/
	if(connect(sock, (struct sockaddr*)&echoServAddr, sizeof(echoServAddr)) < 0)
		DieWithError("connect() failed");

	/*Check Sign*/
	if(send(sock, checkSign, sizeof(checkSign), 0) != sizeof(checkSign))
		DieWithError("check send() failed");
	else
		printf("msg-> %s\n",checkSign);
	if(recv(sock, checkEchoBuffer, RCVBUFSIZE-1, 0) <= 0)
		DieWithError("check recv() failed");
	else
		printf("msg<- %s\n",checkEchoBuffer);

	while(getchar()!='\n');  //stdin flush

	while(1){  //Run until /quit input
		printf("msg-> ");  //string input
		gets(echoString);

		if(strcmp(echoString,"/quit") == 0)
			break;

		echoStringLen = strlen(echoString);  //Determine input length

		/*Send the string to the server*/
		if(send(sock, echoString, echoStringLen, 0) != echoStringLen)
			DieWithError("send() sent a different number of bytes than expected");

		/*Receive the same string back from the server*/
		totalBytesRcvd = 0;
		printf("msg<- ");  //Setup to print the echoed string
		while(totalBytesRcvd < echoStringLen){
			/*Receive up to the buffer size(minus 1 to leave space for a null terminator) bytes from the sender*/
			if((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE-1, 0)) <= 0)
				DieWithError("recv() failed or connection closed prematurely");

			totalBytesRcvd += bytesRcvd;  //Keep tally of tatal bytes
			echoBuffer[bytesRcvd] = '\0';  //Terminate the string!
			printf(echoBuffer);  //Print the echo buffer
		}
		printf("\n");
	}

	printf("\n");
	close(sock);
	exit(0);
}
