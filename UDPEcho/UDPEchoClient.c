#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>

#define BUFSIZE 30

void DieWithError(char*);

int main(int argc, char** argv){
	int sock;
	char message[BUFSIZE];
	int str_len, addr_size;
	
	struct sockaddr_in serv_addr;
	struct sockaddr_in from_addr;

	sock = socket(PF_INET, SOCK_DGRAM, 0);

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	while(1){
		fgets(message, sizeof(message), stdin);
		if(!strcmp(message, "q\n")) break;
		
		sendto(sock, message, strlen(message), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
		addr_size = sizeof(from_addr);
		str_len = recvfrom(sock, message, BUFSIZE, 0, (struct sockaddr*)&from_addr, &addr_size);

		message[str_len] = 0;
		printf("from server: %s",message);
	}
	close(sock);
	return 0;
}
