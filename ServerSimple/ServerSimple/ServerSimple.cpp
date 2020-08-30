// ServerSimple.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <winsock2.h>

#pragma comment(lib,"ws2_32.lib")

//错误码定义
const int ERROR_EXIT = -1;

//服务器配置
const char * ADDR = "0.0.0.0";
const int PORT = 9102;
const int BUF_SIZE = 1024;

//释放套接字资源
void CleanUp(SOCKET serSock, SOCKET cliSock);

int main()
{
	int ret = 0;
	WSADATA wsaData = { 0 };
	SOCKET serSock = INVALID_SOCKET;
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

	//创建服务端的socket
	serSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serSock == INVALID_SOCKET) {
		printf("socket function failed with error = %d\n", WSAGetLastError());
		CleanUp(serSock, INVALID_SOCKET);
		return ERROR_EXIT;
	}

	//命名协议，IP，端口
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(port);
	//serAddr.sin_addr.S_un.S_addr = inet_addr(addr);
	serAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);


	//绑定套接字
	ret = bind(serSock, (SOCKADDR *)&serAddr, sizeof(SOCKADDR_IN));
	if (ret != 0)
	{
		printf("bind socket error: %d\n", ret);
		CleanUp(serSock, INVALID_SOCKET);
		return ERROR_EXIT;
	}

	printf("server[%d] is listening on %s:%d...\n", serSock, addr, port);

	ret = listen(serSock, 5);
	if (ret != 0)
	{
		printf("listen error: %d\n", ret);
		CleanUp(serSock, INVALID_SOCKET);
		return ERROR_EXIT;
	}

	//接受客户端
	len = sizeof(SOCKADDR);
	cliSock = accept(serSock, (SOCKADDR *)&cliAddr, &len);
	if (cliSock == INVALID_SOCKET){
		printf("accept error: %d\n", WSAGetLastError());
		CleanUp(serSock, cliSock);
		return ERROR_EXIT;
	}
	printf("accpet client success, cliSock=%d\n", cliSock);

	//接收数据
	while (1)
	{
		if (recv(cliSock, buf, 1024, 0) <= 0)
		{
			printf("close conn, cliSock=%d\n", cliSock);
			CleanUp(serSock, cliSock);
			return ERROR_EXIT;
		}else {
			printf("recv: cliSock=%d|msg=%s\n", cliSock, buf);
		}
		break;  //只接收一次数据
	}

	//从控制台读取要发送的数据
	printf("Please input any string:\n");
	std::cin >> buf;
	
	//发送数据
	if (send(cliSock, buf, strlen(buf) + 1, 0) <= 0)
	{
		printf("send error, cliSock=%d\n", cliSock);
		CleanUp(serSock, cliSock);
		return ERROR_EXIT;
	}
	else {
		printf("send to client, cliSock=%d|msg=%s", cliSock, buf);
	}

	CleanUp(serSock, cliSock);
	return 0;
}

//释放所有资源
void CleanUp(SOCKET serSock, SOCKET cliSock) {
	closesocket(serSock);
	closesocket(cliSock);
	WSACleanup();
}
