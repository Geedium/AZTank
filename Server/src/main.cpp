#include "stdafx.h"

#include <string>
#include <map>
#include <stack>

constexpr bool			SERVER_DEBUG = 0;
constexpr enet_uint16	SERVER_PORT = 4242;
constexpr size_t		SERVER_MAX_PEERS = 32;
constexpr size_t		SERVER_CHANNELS = 1;

constexpr enet_uint32	PACKET_PLAYER_UPDATE = 3;
constexpr enet_uint32	PACKET_SPAWN_BULLET = 10;

struct ClientData
{
private:
	int id;
public:
	ClientData(int id) : id(id) {}
public:
	int GetID() { return id; }
};

struct Bullet
{
public:
	float x;
	float y;
public:
	Bullet() : x(0), y(0) {}
	Bullet(const float x, const float y) : x(x), y(y) {}
};

std::map<int, ClientData*> client_map;
std::stack<Bullet> bullet_map;

struct PlayerServerData
{
public:
	int id;
	float x;
	float y;
public:
	PlayerServerData(int id) : id(id), x(.0f), y(.0f) {}
};
std::map<int, PlayerServerData*> player_map;

void BroadcastPacket(ENetHost* server, const char* data)
{
	ENetPacket* packet = enet_packet_create(data, strlen(data) + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(server, 0, packet);
}

void BroadcastUFPacket(ENetHost* server, const char* data)
{
	ENetPacket* packet = enet_packet_create(data, strlen(data) + 1, ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT);
	enet_host_broadcast(server, 0, packet);
}

void SendPacket(ENetPeer* peer, const char* data)
{
	ENetPacket* packet = enet_packet_create(data, strlen(data) + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 0, packet);
}

void ParseData(ENetHost* server, int id, const char* data)
{
	int data_type;
	float x, y;
	sscanf(data, "%d|%f|%f", &data_type, &x, &y);

	switch (data_type)
	{
	case 1:
	{
		char msg[80];
		sscanf(data, "%*d|%[^\n]", msg);

		char send_data[1024] = { '\0' };
		sprintf(send_data, "1|%d|%s", id, data);
		BroadcastPacket(server, send_data);

		break;
	}
	case PACKET_PLAYER_UPDATE:
	{
		player_map[id]->x = x;
		player_map[id]->y = y;

		std::string packet = "6|"; // packet type.

		const std::string _id = std::to_string(id); // 2-4 bytes.
		const std::string _x = std::to_string(x); // 4 bytes.
		const std::string _y = std::to_string(y); // 4 bytes.

		packet.append(_id).append("|").append(_x).append("|").append(_y);

		BroadcastUFPacket(server, packet.c_str()); // broadcast packet containing 12 bytes.
		
		break;
	}
	case PACKET_SPAWN_BULLET:

		bullet_map.push(Bullet(x, y));
		std::cout << "Got new bullet!" << std::endl;

		std::string packet = "4|";

		const std::string _id = std::to_string(id);
		const std::string _x = std::to_string(x);
		const std::string _y = std::to_string(y);
	
		packet.append(_id).append("|").append(_x).append("|").append(_y);

		BroadcastPacket(server, packet.c_str());

		break;
	}
}

int main(int argc, char** argv)
{
	if (enet_initialize() != 0)
	{
		fprintf(stderr, "An error occurred while initializing ENet!\n");
		return EXIT_FAILURE;
	}
	atexit(enet_deinitialize);

	ENetAddress address;
	address.host = ENET_HOST_ANY;
	address.port = SERVER_PORT;

	ENetEvent event;

	ENetHost* server = enet_host_create(&address, SERVER_MAX_PEERS, SERVER_CHANNELS, NULL, NULL);
	if (server == NULL)
	{
		fprintf(stderr, "An error occurred while trying to create an ENet server!\n");
		return EXIT_FAILURE;
	}

	int queue = 0;

	while (true)
	{
		while (enet_host_service(server, &event, 1000) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
			{
				printf("A new client connected from %x:%u.\n",
					event.peer->address.host,
					event.peer->address.port);

				queue++;
				client_map[queue] = new ClientData(queue);
				player_map[queue] = new PlayerServerData(queue);
				event.peer->data = client_map[queue];

				char data_to_send[126];
				sprintf(data_to_send, "3|%d", queue);
				SendPacket(event.peer, data_to_send);

				for (auto const& [key, value] : client_map)
				{
					char send_data[512] = { '\0' };
					sprintf(send_data, "2|%d|%f|%f", key, player_map[key]->x, player_map[key]->y);
					BroadcastPacket(server, send_data);
				}

				break;
			}
			case ENET_EVENT_TYPE_RECEIVE:
			{
				if (SERVER_DEBUG)
				{
					printf("A packet of length %u containing %s was received from %x:%u, %u.\n",
						event.packet->dataLength,
						event.packet->data,
						event.peer->address.host,
						event.peer->address.port,
						event.channelID);
				}

				ParseData(server, static_cast<ClientData*>(event.peer->data)->GetID(), (char*)event.packet->data);
				enet_packet_destroy(event.packet);
				break;
			}
			case ENET_EVENT_TYPE_DISCONNECT:
				printf("%x:%u disconnected.\n", event.peer->address.host, event.peer->address.port);
				event.peer->data = NULL;
			}
		}
	}

	enet_host_destroy(server);
	return EXIT_SUCCESS;
}