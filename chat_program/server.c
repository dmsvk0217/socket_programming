#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>
#include <signal.h>
#include <fcntl.h>
#define PORT_NUMBER 7778
#define BACKLOG 10
#define BUFSIZE 65536
#define SEND_MESSAGE_BUFSIZE 1024
char* CURR_MY_PATH_ROOT;

void error_handling(char *);
void GET_handler(char *, char *, char *, int);
void request_handler(void *, struct sockaddr_in*);

int main(int argc, char* argv[]){
  int server_socket;
  int client_socket;
  struct sockaddr_in server_address;
  struct sockaddr_in client_address;
  char message[BUFSIZE];
  int client_address_size;
  int nRcv;

  CURR_MY_PATH_ROOT = getenv("PWD");

  if((server_socket = socket(PF_INET, SOCK_STREAM, 0))==-1){
    perror("socket");
    exit(1);
  }

  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(PORT_NUMBER);
  server_address.sin_addr.s_addr = inet_addr(ANYIP);

  if(bind(server_socket, (struct sockaddr *)&server_address, sizeof(struct sockaddr_in)) == -1){
    perror("bind");
    exit(1);
  }

  if(listen(server_socket, BACKLOG) == -1){
      perror("listen");
      exit(1);
  }

  client_address_size = sizeof(struct sockaddr_in);
  if((client_socket = accept(server_socket, (struct sockaddr*)&client_address, (socklen_t *)&client_address_size)) == -1){
      perror("accept");
  }
  printf("Connection Request : %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
  
  while(1){
    request_handler(&client_socket, &client_address);
  }
  close(client_socket);
  close(server_socket);
  return 0;
}


void request_handler(void *arg, struct sockaddr_in* client_address){
	int client = *(int*)arg;
  char send_message[100], sent_message[100];

	

		//채팅 시작 
		while(1)
		{
			//수신
      int str_len;
			while(str_len <= 0)
			{
				str_len = recv(client, sent_message, BUFSIZE-1, 0);
        if(str_len<=0) error_handling("Error about recv()!!");
        printf("-----------Request message from Client-----------\n");
        printf("%s",sent_message);
        printf("\n-------------------------------------------------\n");
			}

      //종료 조건.
			if(!strcmp(sent_message, "q") || !strcmp(sent_message, "quit"))
			{
				printf("클라이언트로부터 종료메시지를 받았습니다. 채팅프로그램이 종료됩니다.\n");
				break;
			}

      // 받은 메세지 출력하기.
			printf("\n[%s:%d] : %s\n\n", inet_ntoa(client_address->sin_addr),ntohs(client_address->sin_port), sent_message);

		
			// 송신.
			printf("input the message : ");
			fgets(send_message, 100, stdin);
			if(!strcmp(send_message, "q") || !strcmp(send_message, "quit"))
			{
				write(client, send_message, sizeof(send_message));
				printf("채팅 프로그램이 종료됩니다.\n");
				break;
			}
			write(client, send_message, sizeof(send_message));
		}
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
