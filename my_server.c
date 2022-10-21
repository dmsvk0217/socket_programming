#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>
#define PORT_NUMBER 7779
#define BACKLOG 10

int main(){
  int server_socket;
  int client_socket;
  struct sockaddr_in server_address;
  struct sockaddr_in client_address;
  char message[800];
  int str_len;
  int nRcv;

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
    str_len = sizeof(struct sockaddr_in);
    if((client_socket = accept(server_socket, (struct sockaddr*)&client_address, (socklen_t *)&str_len)) == -1){
      perror("accept");
      continue;
    }
    nRcv = recv(client_socket, message, sizeof(message) - 1, 0);
    printf("Receive Message : %s", message);

    printf("server : got connection form %s\n", inet_ntoa(client_address.sin_addr));
  }


  

}