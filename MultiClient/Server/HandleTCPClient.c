#include<stdio.h>
#include<sys/socket.h>
#include<unistd.h>
#include<string.h>

#define RCVBUFSIZE 32  //size of receive buffer

#define EXIT_SIG "quit"  //exit signal
#define FT_SIG "FT"  //File Transfer start signal

void DieWithError(char* errorMessage);  //Error Handling Function
void FileRecv(int clntSock);  //File Receiver

void HandleTCPClient(int clntSocket){
	char buffer[RCVBUFSIZE];  //buffer for string
	int recvMsgSize;  //size of receive message size

	/*Receive message from client*/
	if((recvMsgSize = recv(clntSocket, buffer, RCVBUFSIZE, 0)) < 0)
		DieWithError("recv() failed");

	if(strcmp(buffer,FT_SIG) != 0)
		printf("msg<- %s\n",buffer);
	/*Exit signal input*/
	if(strcmp(buffer, EXIT_SIG) == 0){
		if(send(clntSocket, buffer, recvMsgSize, 0) != recvMsgSize)
			DieWithError("EXIT_SIG send() failed");
		close(clntSocket);
		return;
	}
	/*File transfer signal input*/
	else if(strcmp(buffer, FT_SIG) == 0){
		if(send(clntSocket, buffer, recvMsgSize, 0) != recvMsgSize)
			DieWithError("File transfer send() failed HandleTCPClient.c line 34 ");
		FileRecv(clntSocket);
	}
	/*Send received string and receive again until end of  transmission*/
	while(recvMsgSize > 0){
		/*Message back to client*/
		if(send(clntSocket, buffer, recvMsgSize, 0) != recvMsgSize)
			DieWithError("send() failed");
		else if(strcmp(buffer,FT_SIG) != 0)
			printf("msg-> %s\n",buffer);
		/*See if there is more data to receive*/
		if((recvMsgSize = recv(clntSocket, buffer, RCVBUFSIZE, 0)) < 0)
			DieWithError("recv() failed");
		/*Exit signal input*/
		if(strcmp(buffer,EXIT_SIG) == 0){
			if(send(clntSocket, buffer, recvMsgSize, 0) != recvMsgSize)
				DieWithError("EXIT_SIG send() failed");
			break;
		}
		/*File transfer signal input*/
		else if(strcmp(buffer, FT_SIG) == 0){
			if(send(clntSocket, buffer, recvMsgSize, 0) != recvMsgSize)
				DieWithError("File transfer send() failed HandleTCPClient.c line 34 ");
			FileRecv(clntSocket);
		}
		if(strcmp(buffer,FT_SIG) != 0)
			printf("msg<- %s\n",buffer);
	}
	close(clntSocket);
}
