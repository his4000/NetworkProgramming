#ifndef H_SFTP
#define H_SFTP

#define BUFSIZE 512  //buffer size

#define UP_SIG "p"  //upload signal "put"
#define DOWN_SIG "g"  //download signal "get"
#define EXIT_SIG "e"  //Exit signal "exit"
#define CD "cd" //cd command
#define RCD "rcd"  //rcd command
#define ls "l"  //ls command
#define rls "r"  //rls command
#define ACK_SIG "ACK"  //ACK signal
#define N_ACK_SIG "N_ACK"  //N_ACK signal

#define EchoReq 1
#define FileUpReq 2
#define EchoAck 11
#define FileAck 12

#define SERVER_LIST_FILE "server_list_file.txt"  //server list file
#define CLIENT_LIST_FILE "client_list_file.txt"//client list file

#endif
