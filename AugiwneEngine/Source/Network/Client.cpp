#include <WS2tcpip.h>
#include <iostream>
#include <cstdio>
#include "Client.h"

sockaddr_in server;
SOCKET client;

struct Vec2
{
	float x, y;
};

Client::Client()
{
	WSADATA data;
	WORD version = MAKEWORD(2, 2);
	int wsOk = WSAStartup(version, &data);
	if (wsOk != 0)
	{
		std::cout << "Can't start WinSock! : " << wsOk;
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(42292);
	inet_pton(AF_INET, "85.232.131.161", &server.sin_addr);

	client = socket(AF_INET, SOCK_DGRAM, 0);

	std::string s("Hello world!");

	int sendOk = sendto(client, s.c_str(), s.size() + 1, 0, (sockaddr*)&server, sizeof(server));
	if (sendOk == SOCKET_ERROR)
	{
		std::cout << "Network Exception: " << WSAGetLastError() << std::endl;
	}

	std::cout << "WSA Last Error: " << WSAGetLastError() << std::endl;

	char msg[24];
	int len = sizeof(server);
	int recvOk = recvfrom(client, msg, sizeof(msg) + 1, 0, (sockaddr*)&server, &len);
	std::cout << "NET: " << (recvOk == SOCKET_ERROR ? "FAILED" : "OK") << std::endl;
}

Client::~Client()
{
	closesocket(client);
	WSACleanup();
}

void Client::Send(const float x, const float y)
{
	Vec2 data;
	data.x = x;
	data.y = y;

	char msg[24];
	memcpy(&msg, &data, 24);

	sendto(client, msg, sizeof(msg) + 1, 0, (sockaddr*)&server, sizeof(server));
}

bool isTC;
int _ID = -1;

DWORD WINAPI Receiver(void* arg)
{
	while (_ID == -1)
	{
		char msg[24];
		int len = sizeof(server);
		ZeroMemory(&server, len);

		int recvOk = recvfrom(client, msg, sizeof(msg) + 1, 0, (sockaddr*)&server, &len);
		if (recvOk != SOCKET_ERROR)
		{
			_ID = atoi(msg);
		}
		else
		{
			std::cout << WSAGetLastError() << std::endl;
			throw;
		}
	}

	return EXIT_SUCCESS;
}

int Client::Create()
{
	if (!isTC)
	{
		CreateThread(NULL, 0, Receiver, NULL, 0, NULL);
		isTC = true;
	}
	return _ID;
}