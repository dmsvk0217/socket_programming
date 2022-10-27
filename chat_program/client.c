#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define PORT_NUMBER 7778
#define MESSAGE_BUFSIZE 1024


int main(int argc, char* argv[])
{
	char send_message[MESSAGE_BUFSIZE];
  char sent_message[MESSAGE_BUFSIZE];

	int client_sock;
	struct sockaddr_in server_address;
	int str_len;

	printf("starting your chatting!!---------------------\n\n");

  // client socket 만들기
  client_sock = socket(PF_INET, SOCK_STREAM, 0);
  if(client_sock == -1)
  {
    printf("socket() 에러!!\n");
    return 1;
  }

  // server socket 정보 설정하기.
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(PORT_NUMBER);
  server_address.sin_addr.s_addr = inet_addr(ANYIP);

  // server socket과 연결 connet()
  if(connect(client_sock, (struct sockaddr*)&server_address, sizeof(server_address))==-1)
  {
    printf("connect() 에러!!\n");
    return 1;
  }

  printf("채팅을 시작 - (나가기 q or quit)\n");

  //채팅 시작 
  while(1)
  {
    //송신
    printf("input the message : ");
    fgets(send_message, 100, stdin);

    //채팅 종료조건
    if(!strcmp(send_message, "q") || !strcmp(send_message, "quit"))
    {
      write(client_sock, send_message, sizeof(send_message));
      printf("채팅 프로그램이 종료됩니다.\n");
      break;
    }
    write(client_sock, send_message, sizeof(send_message)); //서버에게 메시지 보냄	

    //수신
    str_len = 0;
    while(str_len <= 0)
    {
      str_len = read(client_sock, sent_message, 100); //메시지 받음
      if(str_len == -1)
      {
        printf("read() 에러!!\n");
        return 1;
      }
    }

    if(!strcmp(sent_message, "q") || !strcmp(sent_message, "quit"))
    {
      printf("서버로 부터 종료메시지를 받았습니다. 채팅프로그램이 종료됩니다.\n");
      break;
    }
    printf("\nServer : %s\n\n", sent_message); //받은 메시지 출력
	}
	//연결끊고 프로그램 종료 
	close(client_sock);
	printf("이용해 주셔서 감사합니다.\n");
	return 0;
}
