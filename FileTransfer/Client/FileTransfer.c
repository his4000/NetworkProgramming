#include<stdio.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/stat.h>

#define RCVBUFSIZE 512  //buffer size
#define ACKBUFSIZE 32  //ACK Buffer size

#define EXIT_SIG "quit"  //exit signal
#define ACK_SIG "ACK"  //ACK signal
#define N_ACK_SIG "N_ACK"  //terminate sign

#define FILE_NAME "test.txt"  //File name

void DieWithError(char* errorMessage);  //Error Handling Function

void FileSend(int servSock){
	char buffer[RCVBUFSIZE];  //buffer for temporal file
	int recvBufSize;  //size of temporal file size
	char A_buffer[ACKBUFSIZE];  //buffer for ack signal
	char ack_sig[ACKBUFSIZE];  //check buffer
	char n_ack_sig[ACKBUFSIZE];  //terminate sign
	int A_BufSize;  //size of ack signal
	FILE* fp;  //File descriptor
	int filesize;  //file size
	char filename[ACKBUFSIZE];  //file name
	struct stat file_info;

	strcpy(ack_sig, ACK_SIG);  //ACK signal
	strcpy(n_ack_sig, N_ACK_SIG);  //N_ACK signal

	/*Intro to File Transfer System*/
	printf("================================================================\n\n");
	printf("   Welcome to Socket File Transfer Client!!!\n\n");

	while(1){
		printf("filename to put to server -> ");
		gets(filename);  //input filename

		/* Exit FT */
		if(strcmp(filename, EXIT_SIG) == 0){
			if(send(servSock, filename, ACKBUFSIZE, 0) != ACKBUFSIZE)
				DieWithError("file Exit signal send() failed in FileTransfer.c line 45");
			return;
		}
		/* No file */
		if(stat(filename, &file_info) < 0)
			printf("There is no file\n");
		/* File Transfer */
		else{
			filesize = file_info.st_size;  //file size input

			/* send file name */
			if(send(servSock, filename, ACKBUFSIZE, 0) != ACKBUFSIZE)
				DieWithError("filename send() failed");
			if((A_BufSize = recv(servSock, A_buffer, ACKBUFSIZE, 0)) < 0)
				DieWithError("filename ACK signal recv() failed");
			if(strcmp(A_buffer, ACK_SIG) != 0)
				DieWithError("ACK signal is different line 47");
			/* send file size */
			if(send(servSock, &filesize, sizeof(filesize), 0) != sizeof(filesize))
				DieWithError("file size send() failed");
			if((A_BufSize = recv(servSock, A_buffer, ACKBUFSIZE, 0)) < 0)
				DieWithError("file size ACK signal recv() failed");
			if(strcmp(A_buffer, ACK_SIG) != 0)	
				DieWithError("ACK signal is different line 56");
	
			/* send ack signal */
			if(send(servSock, ack_sig, ACKBUFSIZE, 0) != ACKBUFSIZE)
				DieWithError("Ack signal send() failed");
			/* recv ack signal */
			if((A_BufSize = recv(servSock, A_buffer, ACKBUFSIZE, 0)) < 0)
				DieWithError("Ack signal recv() failed");
			
			if(strcmp(A_buffer, ACK_SIG) != 0)
				DieWithError("ACK is different line 67");	
			
			/*File Transfer*/
			fp = fopen(filename,"r");
	
			printf("==================== FILE TRANSFER ====================\n");
		
			while(!feof(fp)){
				/*Transfer start sign*/
				if(send(servSock, ack_sig, ACKBUFSIZE, 0) != ACKBUFSIZE)
					DieWithError("transfer start signal send() failed");
				if((A_BufSize = recv(servSock, A_buffer, ACKBUFSIZE, 0)) < 0)
					DieWithError("transfer start signal recv() failed");
				if(strcmp(A_buffer, ACK_SIG) != 0)
					DieWithError("ACK is different");
				/*Transfer start*/
				fgets(buffer, RCVBUFSIZE, fp);  //file read
				if(send(servSock, buffer, RCVBUFSIZE, 0) != RCVBUFSIZE)
					DieWithError("file send() failed");
				printf("%s\n",buffer);  //print file contents
				
				/*confirm ready to send next temporal file*/
				if((A_BufSize = recv(servSock, A_buffer, ACKBUFSIZE, 0)) < 0)
					DieWithError("file ack recv() failed");
				if(strcmp(A_buffer, ACK_SIG) != 0)
					DieWithError("ACK is different");
			}
			printf("=======================================================\n\n");
			printf("   File Name : %s       File Size : %d Bytes\n\n",filename, filesize);
			printf("   File Transfer is Completed...\n\n");
			printf("=======================================================\n\n");
		
			fclose(fp);
		
			/*Terminate File Transfer*/
			if(send(servSock, n_ack_sig, ACKBUFSIZE, 0) != ACKBUFSIZE)
				DieWithError("End transfer signal send() failed");
		}
	}
}
