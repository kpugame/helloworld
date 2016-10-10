#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include<string.h>

//#define SERVERIP   "127.0.0.1"
//#define SERVERPORT 9000
#define BUFSIZE    1024

// ���� �Լ� ���� ��� �� ����
void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// ���� �Լ� ���� ���
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

long GetFileSize(FILE* fp);

int main(int argc, char *argv[])
{
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");
	//printf("%d\n\n", SOMAXCONN);
	char Address[20];
	int port;
	printf("���� ���� �� ��� �����Ǹ� �Է��ϼ��� : ");
	scanf("%s", Address);
	rewind(stdin);
	printf("��Ʈ ��ȣ�� �Է��ϼ��� : ");
	scanf("%d", &port);

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(Address);
	serveraddr.sin_port = htons(port);
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");
	//////////////////////////////////////////////
	int FileSize, FileNameLen;
	FILE* file;
	
	

	char* buf;
	char ch;
	int len, index = 0;
	char FileName[50];
	printf("�����͸� ���� ���� �̸��� �Է��ϼ��� : ");
	scanf("%s", FileName);
	FileNameLen = strlen(FileName);
	file = fopen(FileName, "rt");
	if (file == NULL)
		exit(1);
	
	FileSize = GetFileSize(file);
	
	fclose(file);

	printf("\n%s�� ����ũ�� : %d\n\n", FileName, FileSize);
	// ������ ��ſ� ����� ����


	file = fopen(FileName, "rb+");

	send(sock, (char*)&FileNameLen, sizeof(int), 0);
	send(sock, (char*)FileName, FileNameLen, 0);

	send(sock, (char*)&FileSize, sizeof(int), 0);//����ũ�� ����

	buf = new char[BUFSIZE];
	int i = 0;
	int SendLen = 0;
	//int index = 0;
	while(1)
	{
		ch = fgetc(file);
		i++;
		buf[index] = ch;
		index++;
		if (i >= FileSize)
		{
			send(sock, (char*)&index, sizeof(int), 0);
			printf("%d\n", index);
			send(sock, buf, index, 0);

			break;
		}

	
		if ((index >= BUFSIZE))
		{
			send(sock, (char*)&index, sizeof(int), 0);
			printf("%d\n", index);
			send(sock, buf, index, 0);
			index = 0;
			SendLen += BUFSIZE;
		}
	}
	delete[] buf;
	fclose(file);
	closesocket(sock);

	// ���� ����
	WSACleanup();
	return 0;
}


long GetFileSize(FILE* fp)
{
	long fpos;
	long fsize;

	fpos = ftell(fp);//���� ���������Ͱ� ���ۺ��� �󸶳� �������� ����Ʈ ������ ���ؼ� fpos������ ����
	
	fseek(fp, 0, SEEK_END);
	//printf("fpos = %d\n", fpos);
	
	fsize = ftell(fp);//���� ���� �����Ͱ� ���ۺ��� �󸶳� �������ִ��� ����Ʈ ������ ���ؼ� fsize������ ����
	//fseek(fp, fpos, SEEK_SET); // ���������͸� ������ ������ġ���� fpos��ŭ ������ ��ġ�� �ű�.
	
	printf("fsize = %d\n", fsize);
	return fsize;
}
