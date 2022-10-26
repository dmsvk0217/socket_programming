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
#define PORT_NUMBER 7776
#define BACKLOG 10
#define BUFSIZE 65536
#define SEND_MESSAGE_BUFSIZE 1024
char* CURR_MY_PATH_ROOT;

void error_handling(char *);
void GET_handler(char *, char *, char *, int);
void request_handler(void *);

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
  server_address.sin_addr.s_addr = inet_addr("125.138.72.129");

  if(bind(server_socket, (struct sockaddr *)&server_address, sizeof(struct sockaddr)) == -1){
    perror("bind");
    exit(1);
  }

  if(listen(server_socket, BACKLOG) == -1){
      perror("listen");
      exit(1);
  }

  while(1){
    client_address_size = sizeof(struct sockaddr_in);
    if((client_socket = accept(server_socket, (struct sockaddr*)&client_address, (socklen_t *)&client_address_size)) == -1){
      perror("accept");
      continue;
    }

    printf("Connection Request : %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

    request_handler(&client_socket);
  }
  close(client_socket);
  return 0;
}


void request_handler(void *arg){
  char msg[BUFSIZE];
	char *firstLine[3];

	int sd = *(int*)arg;

	int rcvd = recv(sd, msg, BUFSIZE-1, 0);
	if(rcvd<=0) error_handling("Error about recv()!!");
	printf("-----------Request message from Client-----------\n");
	printf("%s",msg);
	printf("\n-------------------------------------------------\n");

	char post_information[SEND_MESSAGE_BUFSIZE];
	char *curr_msg = NULL;
	char METHOD[4]="";
	char VERSION[10]="";
	char URL[SEND_MESSAGE_BUFSIZE]="";

	curr_msg = strtok(msg, "\n");
	
	int line_count=1;
	while(curr_msg){
		if(line_count>=15) strcpy(post_information, curr_msg);
    printf("[%d]%s\n", line_count, curr_msg);
		curr_msg = strtok(NULL, "\n");
		line_count++;
	}

	firstLine[0] = strtok(msg, " ");
	firstLine[1] = strtok(NULL, " ");
	firstLine[2] = strtok(NULL, " ");
	
	strcpy(METHOD, firstLine[0]);
	strcpy(URL, firstLine[1]);
	strcpy(VERSION, firstLine[2]);
	
  //strncmp 같으면 0 return
	if(!strncmp(METHOD, "GET",3)) GET_handler(VERSION, msg, URL, sd);

	shutdown(sd, SHUT_RDWR);
	close(sd);
}

void GET_handler(char *V, char *message, char *U, int client){
	int fd, str_len;
	char SEND_DATA[SEND_MESSAGE_BUFSIZE];
	char FIANL_PATH[BUFSIZE];
	char VERSION[10]="";
	char URL[SEND_MESSAGE_BUFSIZE]="";
	
	strcpy(VERSION, V);
	strcpy(URL, U);

	if(strncmp(VERSION, "HTTP/1.0",8)!=0 && strncmp(VERSION, "HTTP/1.1",8)!=0){
		write(client, "HTTP/1.1 400 Bad Request\n",25);
	}
	
	if( strlen(URL) == 1 && !strncmp(URL, "/",1)) strcpy(URL, "/index.html");

	strcpy(FIANL_PATH, CURR_MY_PATH_ROOT);
	strcat(FIANL_PATH, URL);

	if((fd=open(FIANL_PATH, O_RDONLY)) != -1){
		send(client, "HTTP/1.0 200 OK\n\n", 17, 0);
		while(1){
			str_len = read(fd, SEND_DATA, SEND_MESSAGE_BUFSIZE);
			if(str_len<=0) break;
			write(client, SEND_DATA, str_len);
		}
	}
	else {
		write(client, "HTTP/1.1 404 Not Found\n", 23);
	}
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
