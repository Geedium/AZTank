#include <iostream>
#include <map>

#include <enet\enet.h>
#include "main.h"

constexpr enet_uint16	SERVER_PORT = 4242;
constexpr size_t		SERVER_MAX_PEERS = 32;
constexpr size_t		SERVER_CHANNELS = 1;

struct ClientData
{
private:
	int id;
public:
	ClientData(int id) : id(id) {}
public:
	int GetID() { return id; }
};

std::map<int, ClientData*> client_map;


void BroadcastPacket(ENetHost* server, const char* data)
{
	ENetPacket* packet = enet_packet_create(data, strlen(data) + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(server, 0, packet);
}

void ParseData(ENetHost* server, int id, char* data)
{
	int data_type;
	sscanf(data, "%d|", &data_type);

	switch (data_type)
	{
	case 1:
		char msg[80];
		sscanf(data, "%*d|%[^\n]", msg);

		char send_data[1024] = { '\0' };
		sprintf(send_data, "1|%d|%s", id, data);
		BroadcastPacket(server, send_data);

		break;
	}
}

void SendPacket(ENetPeer* server, const char* data)
{
	ENetPacket* packet = enet_packet_create(data, strlen(data) + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(server, 0, packet);
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

				for (auto const& x : client_map)
				{
					char send_data[1024] = { '\0' };
					sprintf(send_data, "2|%d", x.first);
					BroadcastPacket(server, send_data);
				}

				queue++;
				client_map[queue] = new ClientData(queue);
				event.peer->data = client_map[queue];

				char data_to_send[126];
				sprintf(data_to_send, "3|%d", queue);
				SendPacket(event.peer, data_to_send);

				break;
			}
			case ENET_EVENT_TYPE_RECEIVE:
			{
				printf("A packet of length %u containing %s was received from %x:%u, %u.\n",
					event.packet->dataLength,
					event.packet->data,
					event.peer->address.host,
					event.peer->address.port,
					event.channelID);

				ParseData(server, static_cast<ClientData*>(event.peer->data)->GetID(), (char*)event.packet->data);
				enet_packet_destroy(event.packet);
				break;
			}
			case ENET_EVENT_TYPE_DISCONNECT:
				printf("%x:%u disconnected.\n",
					event.peer->address.host,
					event.peer->address.port);

				char disconnected_data[126] = { '\0' };
				sprintf(disconnected_data, "4|%d",
					static_cast<ClientData*>(event.peer->data)->GetID());
				BroadcastPacket(server, disconnected_data);

				event.peer->data = NULL;
				break;
			}
		}
	}

	enet_host_destroy(server);
	return EXIT_SUCCESS;
}