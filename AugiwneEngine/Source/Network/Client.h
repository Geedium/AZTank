#pragma once

class Client
{
public:
	Client();
	~Client();
public:
	void Send(const float x, const float y);
	int Create();
};