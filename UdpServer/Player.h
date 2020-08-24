#pragma once

class Player
{
public:
	static std::map<enet_uint8, Player*> objs;
	static enet_uint8 players;
public:
	Player();
	~Player();
public:
	const enet_uint8& GetID() const
	{
		return id;
	}
public:
	float x, y;
private:
	enet_uint8 id;
};