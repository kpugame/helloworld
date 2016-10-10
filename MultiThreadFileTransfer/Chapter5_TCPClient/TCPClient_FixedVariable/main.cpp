#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include<string.h>

//#define SERVERIP   "127.0.0.1"
//#define SERVERPORT 9000
#define BUFSIZE    1024

// 소켓 함수 오류 출력 후 종료
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

// 소켓 함수 오류 출력
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

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");
	//printf("%d\n\n", SOMAXCONN);
	char Address[20];
	int port;
	printf("파일 전송 할 대상 아이피를 입력하세요 : ");
	scanf("%s", Address);
	rewind(stdin);
	printf("포트 번호를 입력하세요 : ");
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
	printf("데이터를 보낼 파일 이름을 입력하세요 : ");
	scanf("%s", FileName);
	FileNameLen = strlen(FileName);
	file = fopen(FileName, "rt");
	if (file == NULL)
		exit(1);
	
	FileSize = GetFileSize(file);
	
	fclose(file);

	printf("\n%s의 파일크기 : %d\n\n", FileName, FileSize);
	// 데이터 통신에 사용할 변수


	file = fopen(FileName, "rb+");

	send(sock, (char*)&FileNameLen, sizeof(int), 0);
	send(sock, (char*)FileName, FileNameLen, 0);

	send(sock, (char*)&FileSize, sizeof(int), 0);//파일크기 전송

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

	// 윈속 종료
	WSACleanup();
	return 0;
}


long GetFileSize(FILE* fp)
{
	long fpos;
	long fsize;

	fpos = ftell(fp);//현재 파일포인터가 시작부터 얼마나 떨어졌나 바이트 단위로 구해서 fpos변수에 저장
	
	fseek(fp, 0, SEEK_END);
	//printf("fpos = %d\n", fpos);
	
	fsize = ftell(fp);//현재 파일 포인터가 시작부터 얼마나 떨어져있는지 바이트 단위로 구해서 fsize변수에 저장
	//fseek(fp, fpos, SEEK_SET); // 파일포인터를 파일의 시작위치에서 fpos만큼 떨어진 위치로 옮김.
	
	printf("fsize = %d\n", fsize);
	return fsize;
}
