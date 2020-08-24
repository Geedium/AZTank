#include "stdafx.h"
#include "Player.h"

std::map<enet_uint8, Player*> Player::objs;
enet_uint8 Player::players;

Player::Player() : QUEUE(id, players)
{
	objs[id] = this;

	x = y = 0;
}

Player::~Player() {}