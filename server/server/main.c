#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<WinSock2.h>
typedef enum { false, true } bool;
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
	SOCKET hServSock, hClntSock;
	SOCKADDR_IN servAddr, clntAddr;

	char message[BUF_SIZE];
	char protocolChar[BUF_SIZE];
	char byte[BUF_SIZE];

	int strLen, i;
	int szClntAddr;
	if (2 != argc)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	hServSock = socket(PF_INET, SOCK_STREAM, 0);
	if (hServSock == INVALID_SOCKET)
		ErrorHandling("socket() error");

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(atoi(argv[1]));

	if (bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
		ErrorHandling("bind() error");
	if (listen(hServSock, 5) == SOCKET_ERROR)
		ErrorHandling("listen() error");
	else
		printf("listen...");

	szClntAddr = sizeof(clntAddr);

	for (i = 0; i < 5; i++)
	{
		hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &szClntAddr);
		if (hClntSock == INVALID_SOCKET)
			ErrorHandling("accept() error");
		else
			printf("Connected client %d \n", i + 1);

		while (1)
		{
			int protocol = -1;
			int recvByte = -1;
			printf("0\n");
			// �������� �ޱ�
			strLen = recv(hClntSock, protocolChar, BUF_SIZE, 0); //----- 1��
			protocolChar[strLen] = 0;
			printf("%s%s%s", "message = ", protocolChar, "__0_\n");
			protocol = atoi(protocolChar);
			printf("1\n");
			// ������ �˻� ���Ұ�� ��������
			if (protocol == PROTOCOL_MESSAGE)
			{
				printf("2\n");
				strLen = recv(hClntSock, message, BUF_SIZE, 0);
				printf("2_\n");
				send(hClntSock, message, strLen, 0);
			}
			else if (protocol == PROTOCOL_DATA) // ������ �˻� �� ��������
			{
				//memset(&message, "", sizeof(BUF_SIZE));
				printf("3\n");
				send(hClntSock, protocolChar, strLen, 0); //----- 2�� �������� �� �޾Ҵٰ� ����
				printf("3_\n");
				strLen = recv(hClntSock, byte, BUF_SIZE, 0);//----- 3�� byte�� ������ ����
				byte[strLen] = 0;
				printf("%s%s%s", "message = ", byte,"__4\n");
				recvByte = atoi(byte);
				send(hClntSock, byte, strLen, 0); //----- 4�� byte �� �޾Ҵٰ� ����
				printf("4_\n");
				strLen = recv(hClntSock, message, BUF_SIZE, 0); //----- 5�� �޽��� ����
				message[strLen] = 0;
				printf("%s%s%s", "message = ", message, "__5\n");
				char result[BUF_SIZE];
				_itoa(strLen, result, 10);
				printf("result = %s", result);
				send(hClntSock, result, strlen(result), 0); //--- 5-1��
				recv(hClntSock, result, BUF_SIZE, 0); //--- 5-2��

				char decision[BUF_SIZE];
				if (recvByte == strLen)
				{
					_itoa(PROTOCOL_SUCCES, decision, 10);
					send(hClntSock, decision, strLen, 0); //-----5-3��
					printf("5-3_\n");
					recv(hClntSock, decision, BUF_SIZE, 0); //---- 5-4��
					printf("6\n");
					send(hClntSock, message, strLen, 0); //----- 6�� �޽��� ����
					printf("6_\n");
				}
				else 
				{
					_itoa(PROTOCOL_FAIL, decision, 10);
					send(hClntSock, decision, strLen, 0); //-----5-3��
					printf("����ġ");
				}
			}
			else if (protocol == PROTOCOL_EXIT)
			{
				printf("����");
				closesocket(hClntSock);
				break;
			}
		}
		
	}

	closesocket(hServSock);
	WSACleanup();
	return 0;
}

void ErrorHandling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}