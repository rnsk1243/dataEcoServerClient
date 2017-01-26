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
	// byte�� char���� ���� �뵵
	char sendByteMessage[BUF_SIZE];
	// ���� �޽��� �뵵�� �˾ƺ��� ���� ���
	char protocol[2];
	// �������� ������ byte�� & ������ ���� ���� byte��
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
			// ���� protocol ������
			send(hClntSock, protocol, strlen(protocol), 0); //----- 1��
			break;
		}
		// �������� �����ų� �޴� byte ���� ����
		sendLen = 0;
		sendLen = strlen(message);
		_itoa(sendLen, sendByteMessage, 10);
		_itoa(PROTOCOL_DATA, protocol, 10);
		// protocol ������
		send(hClntSock, protocol, strlen(protocol), 0); //----- 1��
		//printf("1\n");
		// ������ protocol�� �˾� ������� ���� �ޱ�
		recv(hClntSock, protocol, BUF_SIZE - 1, 0); //----- 2��
		// ���� ���ʹ� �������� ũ�� byte ����
		//printf("1_\n");
		// ������ ������(Ŭ���̾�Ʈ�� ������ ũ��)
		send(hClntSock, sendByteMessage, strlen(sendByteMessage), 0); //----- 3��
		//printf("2\n");
		// ������ �˾� ������� ���� �ޱ�
		recv(hClntSock, sendByteMessage, BUF_SIZE - 1, 0); //----- 4��
		//printf("2_\n");
		// ������ �޽��� ����
		sendLen = send(hClntSock, message, strlen(message), 0); //----- 5��
		//printf("3\n");
		int len = recv(hClntSock, result, BUF_SIZE - 1, 0); //--- 5-1��
		result[len] = 0;
		printf("���� ���� Byte�� = %s\n", sendByteMessage);
		printf("������ ���� Byte�� = %s\n", result);
		send(hClntSock, result, strlen(result), 0); //--- 5-2��
		// �������Լ� ���� ���� byte��
		int strLen = 0;
		// �������Լ� �ѹ� ����  byte��
		int curLen = 0;
		char decisionChar[2];
		recv(hClntSock, decisionChar, BUF_SIZE - 1, 0); //--- 5-3��
		send(hClntSock, decisionChar, strlen(decisionChar), 0);		//---- 5-4��(recv�� ������ ����recv���̿� send����)
		int decision = atoi(decisionChar);
		if (decision == PROTOCOL_SUCCES)
		{
			printf("PROTOCOL_SUCCES\n");
			// ���� byte < ��ü ���� byte �̸�
			while (strLen < sendLen)
			{
				//printf("4\n");
				// ����
				curLen = recv(hClntSock, message, BUF_SIZE - 1, 0); //----- 6��
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
			printf("����ġ �մϴ�...\n");
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