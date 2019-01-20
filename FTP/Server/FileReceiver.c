#include<stdio.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/stat.h>
#include"sftpHdr.h"

void DieWithError(char*);  //Error Handling Function
void recvCommand(int,char*);  //receive command
void sendFile(int,char*,char*,int);  //send file
void recvFile(int,char*,char*,int);  //receive file
void upload(int);  //upload
void download(int);  //download
void makeList(char*);  //make file list
void sendList(int);  //send file list
void RLS_command(int);  //rls command

void FileRecv(int clntSock){
	char buffer[BUFSIZE];  //buffer for temporal file
	int recvBufSize;  //size of temporal file size
	char A_buffer[BUFSIZE];  //buffer for ack signal
	char ack_sig[BUFSIZE];  //check buffer
	char n_ack_sig[BUFSIZE];  //no ack signal
	int A_BufSize;  //size of ack signal
	FILE* fp;
	char filename[BUFSIZE];  //file name
	char filename_buf[BUFSIZE];  //file name buffer
	char command[BUFSIZE];  //command
	int filesize;  //file size
	struct stat file_info;

	strncpy(ack_sig, ACK_SIG, BUFSIZE);  //ACK signal
	strncpy(n_ack_sig,N_ACK_SIG, BUFSIZE);  //N_ACK signal

	while(1){
		/* recv command */
		recvCommand(clntSock,command);
		/* upload */
		if(strcmp(command, UP_SIG) == 0)
			upload(clntSock);
		/* download */
		else if(strcmp(command,DOWN_SIG) == 0)
			download(clntSock);
		/* rls signal input */
		else if(strcmp(command,rls) == 0)
			RLS_command(clntSock);
		/* exit signal input */
		else if(strcmp(command,EXIT_SIG) == 0)
			return;
	}
}

void recvCommand(int clntSock, char* command){
	char ack_sig[BUFSIZE];

	strcpy(ack_sig, ACK_SIG);

	if(recv(clntSock, command, BUFSIZE, 0) < 0)
		DieWithError("command recv() failed in FileRecevier.c line 166");
	if(send(clntSock, ack_sig, BUFSIZE, 0) != BUFSIZE)
		DieWithError("command ack send() failed in FileReceiver.c line 168");	
}

void sendFile(int clntSock, char* filepath, char* filename, int filesize){
	FILE* fp;
	char buffer[BUFSIZE];
	char A_buffer[BUFSIZE];

	fp = fopen(filepath, "r");
	
	printf("========================== FILE TRANSFER ============================\n");
	while(!feof(fp)){
		fgets(buffer,BUFSIZE,fp);
		printf("%s",buffer);
		if(send(clntSock,buffer,BUFSIZE,0) != BUFSIZE)
			DieWithError("file transfer send() failed in FileReceiver.c line 186");
		if(recv(clntSock,A_buffer,BUFSIZE,0) < 0)
			DieWithError("file transfer ACK recv() failed in FileReceiver.c line 188");
	}
	/* send End signal */
	strcpy(A_buffer,N_ACK_SIG);
	if(send(clntSock,A_buffer,BUFSIZE,0) != BUFSIZE)
		DieWithError("file transfer end send() failed in FileReceiver.c line 193");
	if(recv(clntSock,A_buffer,BUFSIZE,0) < 0)
		DieWithError("file transfer end ACK recv() failed in FileRecevier.c line 195");

	fclose(fp);
	printf("\n====================================================================\n\n");
	printf("   File Name : %s    File Size : %d Bytes\n\n",filename,filesize);
	printf("   File Transfer is Completed...\n\n");
	printf("====================================================================\n\n");
}

void recvFile(int clntSock, char* filepath, char* filename, int filesize){
	FILE* fp;
	char buffer[BUFSIZE];
	char ack_sig[BUFSIZE];

	strcpy(ack_sig,ACK_SIG);

	fp = fopen(filepath,"w");

	printf("============================= FILE RECEIVE ============================\n");
	while(1){
		if(recv(clntSock,buffer,BUFSIZE,0) < 0)
			DieWithError("file receive recv() failed in FileReceiver.c line 184");
		if(strcmp(buffer, N_ACK_SIG) == 0)
			break;
		if(send(clntSock,ack_sig,BUFSIZE,0) != BUFSIZE)
			DieWithError("file receive ACK send() failed in FileReceiver.c line 186");
		printf("%s",buffer);
	}
	fclose(fp);

	if(send(clntSock,ack_sig,BUFSIZE,0) != BUFSIZE)
		DieWithError("file receive end ACK send() failed in FileReceiver.c line 194");
	printf("\n=================================================================\n\n");
	printf("   File Name : %s      File Size : %d Bytes\n\n",filename, filesize);
	printf("   File Transfer is Completed...\n\n");
	printf("=================================================================\n\n");
}

void upload(int clntSock){
	char filename[BUFSIZE];
	char filepath[BUFSIZE];
	char ack_sig[BUFSIZE];
	int filesize;

	strcpy(ack_sig, ACK_SIG);

	/* recv file name */
	if(recv(clntSock, filename, BUFSIZE, 0) < 0)
		DieWithError("filename recv() failed");
	if(send(clntSock, ack_sig, BUFSIZE, 0) != BUFSIZE)
		DieWithError("filename ACK signal send() failed");
	/* file name is improper */
	if(strcmp(filename, N_ACK_SIG) == 0)
		return;
	/* recv file path */
	if(recv(clntSock, filepath, BUFSIZE, 0) < 0)
		DieWithError("filepath recv() failed in FileReceiver.c line 187");
	if(send(clntSock, ack_sig, BUFSIZE, 0) != BUFSIZE)
		DieWithError("filepath ACK send() failed in FileReceiver.c line 189");
	printf("%s\n",filepath);
	/* recv file size */
	if(recv(clntSock, &filesize, sizeof(filesize), 0) < 0)
		DieWithError("file size recv() failed");
	if(send(clntSock, ack_sig, BUFSIZE, 0) != BUFSIZE)
		DieWithError("file size ACK signal send() failed");
	/* File Receive */
	recvFile(clntSock, filepath, filename, filesize);	
}

void download(int clntSock){
	char filename[BUFSIZE];
	char filepath[BUFSIZE];
	char ack_sig[BUFSIZE];
	struct stat file_info;
	int filesize;

	strcpy(ack_sig,ACK_SIG);

	/* recv file name */
	if(recv(clntSock, filename, BUFSIZE, 0) < 0)
		DieWithError("filename recv() failed");
	if(send(clntSock, ack_sig, BUFSIZE, 0) != BUFSIZE)
		DieWithError("filename ACK signal send() failed");
	/* file name is improper */
	if(strcmp(filename, N_ACK_SIG) == 0)
		return;
	/* recv file path */
	if(recv(clntSock, filepath, BUFSIZE, 0) < 0)
		DieWithError("filepath recv() failed in FileReceiver.c line 187");
	/* No file */
	if(stat(filepath, &file_info) < 0){
		strcpy(ack_sig,N_ACK_SIG);
		if(send(clntSock, ack_sig, BUFSIZE, 0) != BUFSIZE)
			DieWithError("no file signal send() failed in FileReceiver.c line 297");
		if(recv(clntSock, ack_sig, BUFSIZE, 0) < 0)
			DieWithError("no file signal ACK recv() failed in FileReceiver.c line 299");
		return;
	}
	/* Exist ile */
	filesize = file_info.st_size;
	if(send(clntSock, ack_sig, BUFSIZE, 0) != BUFSIZE)
		DieWithError("file exist send() failed in FileReceiver.c line 304");
	if(recv(clntSock, ack_sig, BUFSIZE, 0) < 0)
		DieWithError("file exist ACK recv() failed in FileReceiver.c line 306");

	/* send file size */
	if(send(clntSock, &filesize, sizeof(int), 0) != sizeof(int))
		DieWithError("filesize send() failed in FileReceiver.c line 316");
	if(recv(clntSock, ack_sig, BUFSIZE, 0) < 0)
		DieWithError("filesize ACK recv() failed in FileReceiver.c line 318");
	/* send file */
	sendFile(clntSock, filepath, filename, filesize);
	
}
void makeList(char* server_directory){
	char path[BUFSIZE] = "ls ";

	strcat(path,server_directory);
	strcat(path," > ");
	strcat(path,SERVER_LIST_FILE);

	system(path);
}
void sendList(int clntSock){
	FILE* fp;
	char buffer[BUFSIZE];
	char A_buffer[BUFSIZE];  //ack buffer
	int A_BufSize;
	char path[BUFSIZE];

	strcpy(A_buffer,ACK_SIG);

	if(send(clntSock, A_buffer, BUFSIZE, 0) != BUFSIZE)
		DieWithError("sendList ack send() failed in FileReceiver.c line 173");
	if(recv(clntSock, path, BUFSIZE, 0) < 0)
		DieWithError("server directory path recv() failed in FileReceiver.c line 176");

	makeList(path);

	fp = fopen(SERVER_LIST_FILE,"r");
	while(fgets(buffer,BUFSIZE,fp)){
		if(send(clntSock, buffer, BUFSIZE, 0) != BUFSIZE)
			DieWithError("list file send() failed in FileReceiver.c line 123");
		if(recv(clntSock, A_buffer, BUFSIZE, 0) < 0)
			DieWithError("next list file request recv() failed in FileReceiver.c line 127");
	}
	strcpy(A_buffer,N_ACK_SIG);  //no ack sig
	if(send(clntSock, A_buffer, BUFSIZE, 0) != BUFSIZE)
		DieWithError("list file end send() failed in FileReceiver.c line 131");

	fclose(fp);
}

void RLS_command(int clntSock){
	sendList(clntSock);
}
