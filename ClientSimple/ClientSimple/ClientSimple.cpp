#include <iostream>
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")


//错误码定义
const int ERROR_EXIT = -1;

//客户端配置
const char * ADDR = "127.0.0.1";
const int PORT = 9102;
const int BUF_SIZE = 1024;

//释放套接字资源
void CleanUp(SOCKET sock);

int main()
{
	int ret = 0;
	WSADATA wsaData = { 0 };
	SOCKET cliSock = INVALID_SOCKET;
	SOCKADDR_IN serAddr = { 0 };
	SOCKADDR_IN cliAddr = { 0 };
	const char * addr = ADDR;
	const int port = PORT;
	char buf[BUF_SIZE] = { 0 };
	int len = 0;

	//初始化套接字库
	ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret != 0) {
		printf("WSAStartup failed: %d\n", ret);
		return ERROR_EXIT;
	}

	//创建客户端的socket
	cliSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (cliSock == INVALID_SOCKET) {
		printf("socket function failed with error = %d\n", WSAGetLastError());
		CleanUp(cliSock);
		return ERROR_EXIT;
	}

	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(port);
	serAddr.sin_addr.S_un.S_addr = inet_addr(addr);

	//请求连接
	ret = connect(cliSock, (SOCKADDR *)&serAddr, sizeof(serAddr));
	if (ret != 0)
	{
		printf("connect failed: %d\n", ret);
		CleanUp(cliSock);
		return ERROR_EXIT;
	}
	printf("connect success\n");

	//从控制台读取要发送的数据
	printf("Please input any string:\n");
	std::cin >> buf;

	//发送数据
	if (send(cliSock, buf, strlen(buf) + 1, 0) <= 0)
	{
		printf("send error\n");
		CleanUp(cliSock);
		return ERROR_EXIT;
	}

	//接收数据
	while (1) {
		if (recv(cliSock, buf, 1024, 0) <= 0)
		{
			printf("recv error\n");
			CleanUp(cliSock);
			return ERROR_EXIT;
		}
		else {
			printf("recv: msg=%s\n", buf);
		}
		break;
	}

	//关闭套接字
	CleanUp(cliSock);
	return 0;
}


//释放所有资源
void CleanUp(SOCKET sock) {
	closesocket(sock);
	WSACleanup();
}
