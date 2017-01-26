#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<WinSock2.h>

#define BUF_SIZE 1024
#define PROTOCOL_DATA 0
#define PROTOCOL_MESSAGE 1
#define PROTOCOL_EXIT 999
#define PROTOCOL_FAIL 2
#define PROTOCOL_SUCCES 3
void ErrorHandling(char* message);

int main(int argc, char* argv[])
{

	WSADATA wsaData;
	SOCKET hClntSock;
	SOCKADDR_IN servAddr;

	char message[BUF_SIZE];
	// byte수 char형에 담을 용도
	char sendByteMessage[BUF_SIZE];
	// 보낸 메시지 용도를 알아보기 위한 약속
	char protocol[2];
	// 서버에게 보내는 byte수 & 서버가 받은 실재 byte수
	char result[BUF_SIZE];
	int sendLen;

	if (3 != argc)
	{
		printf("Usage : %s<IP> <port>\n", argv[0]);
		exit(1);
	}
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	hClntSock = socket(PF_INET, SOCK_STREAM, 0);
	if (hClntSock == INVALID_SOCKET)
		ErrorHandling("socket() error");

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(argv[1]);
	servAddr.sin_port = htons(atoi(argv[2]));

	if (connect(hClntSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
		ErrorHandling("connect() error!");
	else
		puts("Connected.............");

	while (1)
	{
		fputs("Input message(Q to quit): ", stdout);
		fgets(message, BUF_SIZE, stdin);

		if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
		{
			_itoa(PROTOCOL_EXIT, protocol, 10);
			// 종료 protocol 보내기
			send(hClntSock, protocol, strlen(protocol), 0); //----- 1번
			break;
		}
		// 서버에게 보내거나 받는 byte 길이 변수
		sendLen = 0;
		sendLen = strlen(message);
		_itoa(sendLen, sendByteMessage, 10);
		_itoa(PROTOCOL_DATA, protocol, 10);
		// protocol 보내기
		send(hClntSock, protocol, strlen(protocol), 0); //----- 1번
		//printf("1\n");
		// 서버가 protocol을 알아 들었는지 응답 받기
		recv(hClntSock, protocol, BUF_SIZE - 1, 0); //----- 2번
		// 이후 부터는 보낼려는 크기 byte 보냄
		//printf("1_\n");
		// 서버에 보내기(클라이언트가 보내는 크기)
		send(hClntSock, sendByteMessage, strlen(sendByteMessage), 0); //----- 3번
		//printf("2\n");
		// 서버가 알아 들었는지 응답 받기
		recv(hClntSock, sendByteMessage, BUF_SIZE - 1, 0); //----- 4번
		//printf("2_\n");
		// 서버에 메시지 보냄
		sendLen = send(hClntSock, message, strlen(message), 0); //----- 5번
		//printf("3\n");
		int len = recv(hClntSock, result, BUF_SIZE - 1, 0); //--- 5-1번
		result[len] = 0;
		printf("내가 보낸 Byte양 = %s\n", sendByteMessage);
		printf("서버가 받은 Byte양 = %s\n", result);
		send(hClntSock, result, strlen(result), 0); //--- 5-2번
		// 서버에게서 받은 누적 byte양
		int strLen = 0;
		// 서버에게서 한번 받은  byte양
		int curLen = 0;
		char decisionChar[2];
		recv(hClntSock, decisionChar, BUF_SIZE - 1, 0); //--- 5-3번
		send(hClntSock, decisionChar, strlen(decisionChar), 0);		//---- 5-4번(recv를 나누기 위해recv사이에 send껴둠)
		int decision = atoi(decisionChar);
		if (decision == PROTOCOL_SUCCES)
		{
			printf("PROTOCOL_SUCCES\n");
			// 누적 byte < 전체 받을 byte 이면
			while (strLen < sendLen)
			{
				//printf("4\n");
				// 받음
				curLen = recv(hClntSock, message, BUF_SIZE - 1, 0); //----- 6번
																	//printf("5\n");
				if (curLen == -1)
					ErrorHandling("recv() error!");
				strLen += curLen;
			}
			//printf("6\n");
			message[strLen] = 0;
			printf("Message from server: %s\n", message);
		}
		else if (decision == PROTOCOL_FAIL)
		{
			printf("PROTOCOL_FAIL\n");
			printf("불일치 합니다...\n");
		}
		

	}

	closesocket(hClntSock);
	WSACleanup();
	return 0;
}


void ErrorHandling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}