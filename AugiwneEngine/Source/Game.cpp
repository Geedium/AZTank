#include <enet\enet.h>
#include <Augiwne.h>

#define _USE_REMOTE_IP 0
#if _USE_REMOTE_IP
#define IP "85.232.131.161"
#else
#define IP "127.0.0.1"
#endif // _USE_REMOTE_IP

using namespace Augiwne;
using namespace Graphics;

static int CLIENT_ID = -1;


constexpr enet_uint32	PACKET_PLAYER_UPDATE = 6;

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

struct PlayerData
{
public:
	int id;
	bool created;
	Vector2 lastpos;
	Vector2 pos;
	Sprite* object;
public:
	PlayerData(const int id, const float x, const float y)
		: id(id), lastpos(x, y), pos(x, y), created(false), object(nullptr)
	{}
};

std::map<int, PlayerData*> player_map;

Texture* playerTexture;
ENetPeer* peer;

void SendUFPacket(ENetPeer* peer, const char* data)
{
	ENetPacket* packet = enet_packet_create(data, strlen(data) + 1, ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT);
	enet_peer_send(peer, 0, packet);
}

void SendPacket(ENetPeer* peer, const char* data)
{
	ENetPacket* packet = enet_packet_create(data, strlen(data) + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 0, packet);
}

void ParseData(char* data)
{
	enet_uint32 data_type;
	enet_uint32 id;
	float x, y;

	sscanf(data, "%d|%d|%f|%f", &data_type, &id, &x, &y);

	switch (data_type)
	{
	case 2:
		std::cout << "New player create." << std::endl;

		if (id != CLIENT_ID)
		{
			client_map[id] = new ClientData(id);
			player_map[id] = new PlayerData(id, x, y);
		}
		break;
	case 3:
		std::cout << "Client ID got." << std::endl;
		CLIENT_ID = id;
		break;
	case PACKET_PLAYER_UPDATE:
		if (id != CLIENT_ID)
		{
			player_map[id]->pos = Vector2(x, y);
		}
		break;
	}
}

DWORD WINAPI NetLoop(__in LPVOID lpParameter)
{
	if (enet_initialize() != 0)
	{
		fprintf(stderr, "An error occurred while initializng ENet!\n");
		exit(EXIT_FAILURE);
	}
	atexit(enet_deinitialize);

	ENetHost* client = enet_host_create(NULL, 1, 1, 0, 0);

	ENetAddress address;
	ENetEvent event;

	if (client == NULL)
	{
		fprintf(stderr, "An error occurred while trying to create an ENet client!\n");
		exit(EXIT_FAILURE);
	}

	enet_address_set_host(&address, IP);
	address.port = 4242;

	peer = enet_host_connect(client, &address, 1, NULL);
	if (peer == NULL)
	{
		fprintf(stderr, "No available peers for initiating an ENet connection!\n");
		exit(EXIT_FAILURE);
	}

	if (enet_host_service(client, &event, 5000) > 0 &&
		event.type == ENET_EVENT_TYPE_CONNECT)
	{
		puts("Connection to 127.0.0.1:4242 succeeded!");

	}
	else
	{
		enet_peer_reset(peer);
		puts("Connection to 127.0.0.1:4242 failed!");
	}
	
	while (true)
	{
		while (enet_host_service(client, &event, 1000) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_RECEIVE:
				ParseData((char*)event.packet->data);
				enet_packet_destroy(event.packet);
				break;
			}
		}
	}

	enet_peer_disconnect(peer, 0);

	while (enet_host_service(client, &event, 3000) > 0)
	{
		switch (event.type)
		{
		case ENET_EVENT_TYPE_RECEIVE:
			enet_packet_destroy(event.packet);
			break;
		case ENET_EVENT_TYPE_DISCONNECT:
			puts("Disconnection succeeded.");
			break;
		}
	}
}

void SendPosPacket(const float x, const float y)
{
	std::string packet = "3|"; // packet type.

	const std::string _x = std::to_string(x); // 4 bytes.
	const std::string _y = std::to_string(y); // 4 bytes.

	packet.append(_x).append("|").append(_y);

	SendUFPacket(peer, packet.c_str()); // send packet containing 8 bytes.
}

class Game : public Augiwne
{
private:
	Window* window;
private:
	Layer* foreground;
	Layer* players;
private:
	Sprite* player;
	Sprite* sprWolf;
	Shader* diffuse;
	Camera* cam;
	Texture* wolf[4];
	float timer;
	float speed = 0.35f;
	bool moved;
	int sequence;
private:
	enum Direction
	{
		UP,
		DOWN,
		RIGHT,
		LEFT
	};
	Direction heading;
	bool bouncing;
public:
	const bool AABBCollision(Renderable2D* a, Renderable2D* b) const
	{
		bool cx = a->GetPosition().x + a->GetSize().x >= b->GetPosition().x && b->GetPosition().x + b->GetSize().x >= a->GetPosition().x;
		bool cy = a->GetPosition().y + a->GetSize().y >= b->GetPosition().y && b->GetPosition().y + b->GetSize().y >= a->GetPosition().y;
		return cx && cy;
	}
public:
	Game()
	{
		DWORD threadID;
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)NetLoop, NULL, 0, &threadID);
		std::cout << "Background Thread Created: " << threadID << std::endl;
	}
	~Game()
	{

	}
	int GetVSync() {
		ifstream file("cfg/graphics.ini");
		int vsyncInterval = 0;

		if (file.is_open())
		{
			std::string line;
			while (getline(file, line)) {
				line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
				if (line[0] == '#' || line.empty())
					continue;
				auto delimiterPos = line.find("=");
				auto name = line.substr(0, delimiterPos);
				auto value = line.substr(delimiterPos + 1);

				if (name == "vsync")
				{
					vsyncInterval = stoi(value);
				}
			}
			file.close();
		}
		return vsyncInterval;
	}
	void Init() override
	{
		window = CreateRenderWindow("AZ", 960, 540, GetVSync());
		diffuse = new Shader("Data/Vertex.shader", "Data/Frag.shader");
		Matrix4& ortho = Matrix4::Orthographic(-16, 16, -9, 9, -1, 1);
		cam = new Camera(ortho);
		
		foreground = new Layer(new BatchRenderer2D(), diffuse, ortho);
		players = new Layer(new BatchRenderer2D(), diffuse, ortho);

		playerTexture = new Texture("Data/textures/Paddle.png");
		player = new Sprite(-16.6f, 0, 1.75f, 1.75f, playerTexture);
	
		Texture* wallTexture = new Texture("Data/textures/Wall.png");

		for (float x = -16.0f; x < 16.0f; x += 1.0f)
		{
			for (float y = -9.0f; y < 9.0f; y += 1.0f)
			{
				foreground->Add(new Sprite(x, y, 1.0f, 1.0f, wallTexture));
			}
		}

		foreground->Add(player);

		diffuse->Enable();
		diffuse->SetUniformMatrix4("vw_matrix", Matrix4::Orthographic(-1.1, 1.1, -1.1, 1.1, -1, 1));
	}
	float now, last;
	float delta;
	void Tick() override
	{
		printf("%u ups, %u fps\n", GetUPS(), GetFPS());
	}

	float tx = .4f;
	float ty = 0;
	float brk = 0;
	float brk2 = 0;

	void Update() override
	{
		if (now > last)
		{
			delta = ((now - last) / 0.01f);
			last = now;
		}
		now = m_Watcher->Elapsed();

		/*

			BEGIN LOCAL_PLAYER

			*/
		Vector3 pos = player->GetPosition();

		if (window->IsKeyPressed(GLFW_KEY_W))
		{
			pos.y += speed * delta;
		}
		else if (window->IsKeyPressed(GLFW_KEY_S))
		{
			pos.y -= speed * delta;
		}

		if (window->IsKeyPressed(GLFW_KEY_A))
		{
			pos.x -= speed * delta;
		}
		else if (window->IsKeyPressed(GLFW_KEY_D))
		{
			pos.x += speed * delta;
		}

		player->SetPosition(pos);
		SendPosPacket(pos.x, pos.y);

		for (auto& [key, value] : player_map)
		{
			if (!value->created)
			{
				value->created = true;

				value->object = new Sprite(-16.6f, 0.0f, 1.75f, 1.75f, playerTexture);
				value->object->SetPosition(value->pos);

				players->Add(value->object);
			}
			else
			{
				Vector2 captured = value->pos;

				if (value->lastpos != captured)
				{
					Vector2 subtract = captured - value->lastpos;
					Vector2 multiply = subtract.Multiply((now - last) / speed);
					Vector2 lerp = value->lastpos + multiply;
					value->object->SetPosition(lerp);

					value->lastpos = lerp;
				}
			}
		}

		for (int i = 0; i < foreground->GetRenderables().size(); i++)
		{
			diffuse->Enable();

			Renderable2D* rend = foreground->GetRenderables().at(i);
		}
	}

	void Render() override
	{
		double x, y;
		window->GetMousePosition(x, y);

		// - Lightning
		diffuse->Enable();
		diffuse->SetUniform2f("light_pos",
			Vector2((float)(x * 32 / window->GetWidth() - 16),
			(float)(9 - y * 18 / window->GetHeight())));
		diffuse->Enable();


		foreground->Render();
		players->Render();
	}
};

int main()
{
	Game game;
	game.Start();
	return 0;
}