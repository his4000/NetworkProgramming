#include<stdio.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#define RCVBUFSIZE 512  //buffer size
#define ACKBUFSIZE 32  //ACK Buffer size

#define EXIT_SIG "quit"  //exit signal
#define ACK_SIG "ACK"  //ACK signal
#define N_ACK_SIG "N_ACK"  //terminate sign

void DieWithError(char* errorMessage);  //Error Handling Function

void FileRecv(int clntSock){
	char buffer[RCVBUFSIZE];  //buffer for temporal file
	int recvBufSize;  //size of temporal file size
	char A_buffer[ACKBUFSIZE];  //buffer for ack signal
	char ack_sig[ACKBUFSIZE];  //check buffer
	int A_BufSize;  //size of ack signal
	FILE* fp;
	char filename[ACKBUFSIZE];  //file name
	int filesize;  //file size

	strncpy(ack_sig, ACK_SIG, RCVBUFSIZE);  //ACK signal

	while(1){
		/* recv file name */
		if((A_BufSize = recv(clntSock, filename, ACKBUFSIZE, 0)) < 0)
			DieWithError("filename recv() failed");
		if(strcmp(filename, EXIT_SIG) == 0)
			return;
		if(send(clntSock, ack_sig, ACKBUFSIZE, 0) != ACKBUFSIZE)
			DieWithError("filename ACK signal send() failed");
		/* recv file size */
		if((A_BufSize = recv(clntSock, &filesize, sizeof(filesize), 0)) < 0)
			DieWithError("file size recv() failed");
		if(send(clntSock, ack_sig, ACKBUFSIZE, 0) != ACKBUFSIZE)
			DieWithError("file size ACK signal send() failed");
		/* recv ack signal */
		if((A_BufSize = recv(clntSock, A_buffer, ACKBUFSIZE, 0)) < 0)
			DieWithError("Ack signal recv() failed");
		if(strcmp(A_buffer,ACK_SIG) != 0)
			DieWithError("ACK is different FileReceiver.c line 46");
		/* send ack signal */
		if(send(clntSock, ack_sig, ACKBUFSIZE, 0) != ACKBUFSIZE)
			DieWithError("Ack signal send() failed");

		/* File Receive */
		fp = fopen(filename,"w");
	
		printf("======================== FILE RECEIVE ============================\n");
	
		while(1){
			/*Transfer start sign*/
			if((recv(clntSock, A_buffer, ACKBUFSIZE, 0)) < 0)
				DieWithError("transfer start signal recv() failed");
			if(strcmp(A_buffer, N_ACK_SIG) == 0)
				break;
			else if(strcmp(A_buffer, ACK_SIG) != 0)
				DieWithError("ACK is different");
			if(send(clntSock, ack_sig, ACKBUFSIZE, 0) != ACKBUFSIZE)
				DieWithError("transfer start signal send() failed");
			/*Receive start*/
			if((recvBufSize = recv(clntSock, buffer, RCVBUFSIZE, 0)) < 0)
				DieWithError("file recv() failed");
			printf("%s\n",buffer);  //print file contents
			fputs(buffer,fp);  //file input
	
			/*confirm ready to send next temporal file*/
			if(send(clntSock, ack_sig, ACKBUFSIZE, 0) != ACKBUFSIZE)
				DieWithError("file ack send() failed");
		}
		printf("=================================================================\n\n");
		printf("   File Name : %s      File Size : %d Bytes\n\n",filename, filesize);
		printf("   File Transfer is Completed...\n\n");
		printf("=================================================================\n\n");
		fclose(fp);
	}
}
