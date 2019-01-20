#include<stdio.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/stat.h>
#include"sftpHdr.h"

#define RCVBUFSIZE 512  //buffer size
#define ACKBUFSIZE 100  //ACK Buffer size

void DieWithError(char* errorMessage);  //Error Handling Function
void sendCommand(int,char*);  //send command
void sendFile(int,char*,char*,int);  //send file
void recvFile(int,char*,char*,int);  //recv file
void upload(int,char*,char*);  //upload
void download(int,char*,char*);  //download
void recvList(int,char*);  //list file receive
void printList(char*,char*);  //print file list
int findFile(char*);  //find file in list
int isFile(char*);  //is file exist?
void CD_command(char*,char*);  //cd command
void RCD_command(char*,char*);  //rcd command
void LS_command(char*);  //ls command
void RLS_command(int,char*);  //rls command

void FileSend(int servSock){
	char buffer[RCVBUFSIZE];  //buffer for temporal file
	int recvBufSize;  //size of temporal file size
	char A_buffer[ACKBUFSIZE];  //buffer for ack signal
	char ack_sig[ACKBUFSIZE];  //check buffer
	char n_ack_sig[ACKBUFSIZE];  //terminate sign
	int A_BufSize;  //size of ack signal
	FILE* fp;  //File descriptor
	int filesize;  //file size
	char filename[ACKBUFSIZE];  //filename buffer
	char filepath[ACKBUFSIZE];  //file path
	char server_directory[ACKBUFSIZE] = "./storage";  //server side directory path
	char client_directory[ACKBUFSIZE] = "./storage";  //client side directory path
	char command[ACKBUFSIZE];  //command download or upload
	struct stat file_info;

	strcpy(ack_sig, ACK_SIG);  //ACK signal
	strcpy(n_ack_sig, N_ACK_SIG);  //N_ACK signal

	/*Intro to File Transfer System*/
	printf("================================================================\n\n");
	printf("   Welcome to Socket File Transfer Client!!!\n\n");

	while(1){
		printf("ftp command [p)ut  g)et  l)s  r)ls  e)xit] -> ");
		gets(command);  //get command
		/* cd */
		if(command[0] == 'c' && command[1] == 'd' && command[2] == ' ')
			CD_command(client_directory, command);
		/* rcd */
		else if(command[0] == 'r' && command[1] == 'c' && command[2] == 'd' && command[3] == ' ')
			RCD_command(server_directory, command);
		/* ls */
		else if(strcmp(command,ls) == 0)
			LS_command(client_directory);
		/* rls */
		else if(strcmp(command,rls) == 0){
			sendCommand(servSock,command);
			RLS_command(servSock,server_directory);
		}
		/* upload */
		else if(strcmp(command,UP_SIG) == 0){
			/* send command */
			sendCommand(servSock,command);
			/* file upload */
			upload(servSock,client_directory,server_directory);
		}
		/* Download */
		else if(strcmp(command,DOWN_SIG) == 0){
			/* Send Command */
			sendCommand(servSock, command);
			/* download */
			download(servSock, client_directory, server_directory);
		}
		/* exit signal input */
		else if(strcmp(command,EXIT_SIG) == 0){
			if(send(servSock, command, ACKBUFSIZE, 0) != ACKBUFSIZE)
				DieWithError("quit signal send() failed in FileTransfer.c line 138");
			if((A_BufSize = recv(servSock, A_buffer, ACKBUFSIZE, 0)) < 0)
				DieWithError("quit signal ack recv() failed in FielTransfer.c line 143");
			return;
		}
		/* input command error */
		else
			printf("That's not available command\n");
	}
}

void sendCommand(int servSock, char* command){
	char A_buffer[BUFSIZE];

	if(send(servSock, command, BUFSIZE, 0) != BUFSIZE)
		DieWithError("command send() failed in FileTransfer.c line 236");
	if(recv(servSock, A_buffer, BUFSIZE, 0) < 0)
		DieWithError("command ack recv() failed in FileTransfer.c line 238");
}

void sendFile(int servSock, char* filepath, char* filename, int filesize){
	FILE* fp;
	char buffer[BUFSIZE];
	char A_buffer[BUFSIZE];

	fp = fopen(filepath, "r");
	
	printf("========================= FILE TRANSFER =============================\n");
	while(!feof(fp)){
		fgets(buffer,BUFSIZE,fp);
		printf("%s",buffer);
		if(send(servSock,buffer,BUFSIZE,0) != BUFSIZE)
			DieWithError("file transfer send() failed in FileTransfer.c line 254");
		if(recv(servSock,A_buffer,BUFSIZE,0) < 0)
			DieWithError("file transfer ACK recv() failed in FileTransfer.c line 256");
	}
	/* send End signal */
	strcpy(A_buffer,N_ACK_SIG);
	if(send(servSock, A_buffer, BUFSIZE, 0) != BUFSIZE)
		DieWithError("file transfer end send() failed in FileTransfer.c line 261");
	if(recv(servSock, A_buffer, BUFSIZE, 0) < 0)
		DieWithError("file transfer end ACK recv() failed in FileTransfer.c line 263");

	fclose(fp);	
	printf("\n=======================================================\n\n");
	printf("   File Name : %s       File Size : %d Bytes\n\n",filename, filesize);
	printf("   File Transfer is Completed...\n\n");
	printf("=======================================================\n\n");
}

void recvFile(int servSock, char* filepath, char* filename, int filesize){
	FILE* fp;
	char buffer[BUFSIZE];
	char ack_sig[BUFSIZE];

	strcpy(ack_sig,ACK_SIG);

	fp = fopen(filepath,"w");
	
	printf("========================= FILE RECEIVE =========================\n");
	while(1){
		if(recv(servSock,buffer,BUFSIZE,0) < 0)
			DieWithError("file receive recv() failed in FileTransfer.c line 289");
		if(strcmp(buffer, N_ACK_SIG) == 0)
			break;
		if(send(servSock,ack_sig,BUFSIZE,0) != BUFSIZE)
			DieWithError("file receive ACK send() failed in FileTransfer.c line 293");
		printf("%s",buffer);
	}
	fclose(fp);

	if(send(servSock,ack_sig,BUFSIZE,0) != BUFSIZE)
		DieWithError("file receive end ACK send() failed in FileReceiver.c line 298");
	printf("\n================================================================\n\n");
	printf("   File Name : %s     File Size : %d Bytes\n\n",filename, filesize);
	printf("================================================================\n\n");
}

void upload(int servSock, char* client_directory, char* server_directory){
	char filename[BUFSIZE];
	char client_filepath[BUFSIZE];
	char server_filepath[BUFSIZE];
	char A_buffer[BUFSIZE];
	struct stat file_info;	
	int filesize;

	printf("filename to put to server -> ");
	scanf("%s",filename);
	while(getchar()!='\n');

	/* file name is improper */
	if(isFile(filename) == 0){
		printf("File name is improper!!!\n");
		strcpy(A_buffer,N_ACK_SIG);
		if(send(servSock, A_buffer, BUFSIZE, 0) != BUFSIZE)
			DieWithError("file name is improper send() failed in FileTransfer.c line 292");
		if(recv(servSock, A_buffer, BUFSIZE, 0) < 0)
			DieWithError("file name is improper recv() failed in FileTransfer.c line 294");
		return;
	}

	/* make client directory file path */
	strcpy(client_filepath, client_directory);
	strcat(client_filepath, "/");
	strcat(client_filepath, filename);
	/* make server directory file path */
	strcpy(server_filepath, server_directory);
	strcat(server_filepath, "/");
	strcat(server_filepath, filename);
	/* No file */	
	if(stat(client_filepath, &file_info) < 0){
		strcpy(A_buffer,N_ACK_SIG);
		if(send(servSock,A_buffer,BUFSIZE,0) != BUFSIZE)
			DieWithError("no file send() failed in FileTransfer.c line 361");
		if(recv(servSock,A_buffer,BUFSIZE,0) < 0)
			DieWithError("no file recv() failed in FeilTransfer.c line 363");
		printf("There is no file\n");
	}
	/* File Transfer */
	else{
		filesize = file_info.st_size;  //file size input
		
		/* send file name */
		if(send(servSock, filename, BUFSIZE, 0) != BUFSIZE)
			DieWithError("filename send() failed");
		if(recv(servSock, A_buffer, BUFSIZE, 0) < 0)
			DieWithError("filename ACK signal recv() failed");
		/* send file path */
		if(send(servSock, server_filepath, BUFSIZE, 0) != BUFSIZE)
			DieWithError("filepath send() failed in FileTransfer.c line 279");
		if(recv(servSock, A_buffer, BUFSIZE, 0) < 0)
			DieWithError("filepath ACK recv() failed in FileTransfer.c line 281");
		/* send file size */
		if(send(servSock, &filesize, sizeof(filesize), 0) != sizeof(filesize))
			DieWithError("file size send() failed");
		if(recv(servSock, A_buffer, BUFSIZE, 0) < 0)
			DieWithError("file size ACK signal recv() failed");
		/*File Transfer*/
		sendFile(servSock, client_filepath, filename, filesize);
	}
}

void download(int servSock, char* client_directory, char* server_directory){
	char filename[BUFSIZE];
	char client_filepath[BUFSIZE];
	char server_filepath[BUFSIZE];
	char A_buffer[BUFSIZE];
	int filesize;

	printf("filename to get from server -> ");
	scanf("%s",filename);
	while(getchar()!='\n');

	/* file name is improper */
	if(isFile(filename) == 0){
		printf("File name is improper!!!\n");
		strcpy(A_buffer,N_ACK_SIG);
		if(send(servSock, A_buffer, BUFSIZE, 0) != BUFSIZE)
			DieWithError("file name is improper send() failed in FileTransfer.c line 292");
		if(recv(servSock, A_buffer, BUFSIZE, 0) < 0)
			DieWithError("file name is improper recv() failed in FileTransfer.c line 294");
		return;
	}
	/* make client directory file path */
	strcpy(client_filepath, client_directory);
	strcat(client_filepath, "/");
	strcat(client_filepath, filename);
	/* make server directory file path */
	strcpy(server_filepath, server_directory);
	strcat(server_filepath, "/");
	strcat(server_filepath, filename);

	/* send file name */
	if(send(servSock, filename, BUFSIZE, 0) != BUFSIZE)
		DieWithError("file name send() failed in FileTransfer.c line 381");
	if(recv(servSock, A_buffer, BUFSIZE, 0) < 0)
		DieWithError("file name ACK recv() failed in FileTransfer.c line 384");
	/* send file path */
	if(send(servSock, server_filepath, BUFSIZE, 0) != BUFSIZE)
		DieWithError("file path send() failed in FileTransfer.c line 386");
	/* recv file exist */
	if(recv(servSock, A_buffer, BUFSIZE, 0) < 0)
		DieWithError("file exist recv() failed in FileTransfer.c line 416");
	if(send(servSock, A_buffer, BUFSIZE, 0) != BUFSIZE)
		DieWithError("file exist ACK send() failed in FileTransfer.c line 418");
	if(strcmp(A_buffer, N_ACK_SIG) == 0)  //file is not exist
		return;
	/* recv filesize */
	if(recv(servSock, &filesize, sizeof(int), 0) < 0)
		DieWithError("file size recv() failed in FileTransfer.c line 435");
	if(send(servSock, A_buffer, BUFSIZE, 0) != BUFSIZE)
		DieWithError("file size ACK send() failed in FileTransfer.c line 437");
	/* receive file */
	recvFile(servSock, client_filepath, filename, filesize);	
}

void recvList(int servSock, char* server_directory){
	FILE* fp;
	char buffer[RCVBUFSIZE];
	char A_buffer[RCVBUFSIZE];
	int recvBufSize;

	strncpy(A_buffer,ACK_SIG,BUFSIZE);  //ack signal

	if(recv(servSock, A_buffer, BUFSIZE, 0) < 0)
		DieWithError("recvList ack recv() failed in FileTransfer.c line 241");
	if(send(servSock, server_directory, BUFSIZE, 0) != BUFSIZE)
		DieWithError("server directory path send() failed in FileTransfer.c line 243");

	fp = fopen(SERVER_LIST_FILE, "w");

	while(1){
		if(recv(servSock, buffer, BUFSIZE, 0) < 0)
			DieWithError("list file recv() failed in FileTransfer.c line 169");
		if(strcmp(buffer,N_ACK_SIG) == 0)
			break;
		fputs(buffer,fp);
		if(send(servSock, A_buffer, BUFSIZE, 0) != BUFSIZE)
			DieWithError("next list file send() failed in FileTransfer.c line 174");
	}
	fclose(fp);
}

void printList(char* dir, char* path){
	FILE* fp;
	char buffer[RCVBUFSIZE];

	printf("\n\n---------- File List in %s -----------\n\n",path);

	fp = fopen(dir,"r");
	
	while(fgets(buffer,RCVBUFSIZE,fp))
		printf("%s\n",buffer);

	fclose(fp);

	printf("\n\n--------------------------------------------------\n\n");
}

int findFile(char* filename){
	FILE* fp;
	char buffer[RCVBUFSIZE];

	fp = fopen(SERVER_LIST_FILE,"r");
	
	while(!feof(fp)){
		fgets(buffer,RCVBUFSIZE,fp);  //read file list line
		if(strncmp(buffer, filename, strlen(filename)) == 0){
			if(isFile(filename))
				return 1;
		}
	}

	fclose(fp);

	return 0;
}

int isFile(char* filename){
	int i;

	for(i=0;i<strlen(filename);i++){
		if(filename[i] == '.')
			return 1;
	}
	return 0;
}

void CD_command(char* client_directory, char* command){
	char path[ACKBUFSIZE];
	int i;

	for(i=0;i<strlen(command)-3;i++)
		path[i] = command[i+3];
	path[i] = '\0';

	/* goto prev directory */
	if(path[0] == '.' && path[1] == '.'){
		for(i=strlen(client_directory);client_directory[i]!='/';i--);
		client_directory[i] = '\0';
		return;
	}
	/* it's not directory */
	if(isFile(path))
		return;
	strcat(client_directory, "/");
	strcat(client_directory, path);
}

void RCD_command(char* server_directory, char* command){
	char path[ACKBUFSIZE];
	int i;
	
	for(i=0;i<strlen(command)-4;i++)
		path[i] = command[i+4];
	path[i] = '\0';

	/* goto prev directory */
	if(path[0] == '.' && path[1] == '.'){
		for(i=strlen(server_directory);server_directory[i]!='/';i--);
		server_directory[i] = '\0';
		return;
	}
	/* it's not directory */
	if(isFile(path))
		return;
	strcat(server_directory, "/");
	strcat(server_directory, path);
}

void LS_command(char* client_directory){
	char path[ACKBUFSIZE] = "ls ";
	FILE* fp;

	strcat(path, client_directory);
	strcat(path, " > ");
	strcat(path, CLIENT_LIST_FILE);

	system(path);

	printList(CLIENT_LIST_FILE, client_directory);	
}

void RLS_command(int servSock, char* server_directory){
	recvList(servSock, server_directory);
	printList(SERVER_LIST_FILE, server_directory);
}
