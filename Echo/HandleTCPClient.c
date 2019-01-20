#include<stdio.h>
#include<sys/socket.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>

#define RCVBUFSIZE 32  //Size of receive buffer

void DieWithError(char* errorMessage);  //Error handling function

void HandleTCPClient(int clntSocket){
	char echoBuffer[RCVBUFSIZE];  //Buffer for echo string
	int recvMsgSize;  //Size of received message

	/*Receive message from client*/
	if((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
		DieWithError("recv() failed");

	if(recvMsgSize == 0){
		close(clntSocket);  //Close client socket
		return;
	}
	
	echoBuffer[recvMsgSize] = '\0';  //string terminated
	printf("msg<- %s\n",echoBuffer);  //Message print in Server Side

	/*Send received string and receive again until end of transmission*/
	while(recvMsgSize > 0){  //zero indicates end of transmission
		/*Echo message back to client*/
		if(send(clntSocket, echoBuffer, recvMsgSize, 0) != recvMsgSize)
			DieWithError("send() failed");
		else
			printf("msg-> %s\n",echoBuffer);  //Messge print in Server Side
		/*See if there is more data to receive*/
		if((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0){
			DieWithError("recv() failed");
		}
		
		echoBuffer[recvMsgSize] = '\0';  //string terminated
		if(recvMsgSize > 0)
			printf("msg<- %s\n",echoBuffer);  //Message print in Server Side
	}
	close(clntSocket);  //Close client socket
}
