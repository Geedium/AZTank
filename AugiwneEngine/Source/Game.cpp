#include <enet\enet.h>
#include <Augiwne.h>

using namespace Augiwne;
using namespace Graphics;

static int CLIENT_ID = -1;

struct ThreadParams
{
	ENetHost* client;
};

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
Texture* playerTexture;
Layer* layer;

void SendPacket(ENetPeer* peer, const char* data)
{
	ENetPacket* packet = enet_packet_create(data, strlen(data) + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 0, packet);
}

void ParseData(char* data)
{
	int data_type;
	int id;

	sscanf(data, "%d|%d", &data_type, &id);

	switch (data_type)
	{
	case 1:

		break;
	case 2:
		std::cout << "Client map request." << std::endl;

		if (id != CLIENT_ID)
		{
			client_map[id] = new ClientData(id);
			layer->Add(new Sprite(-16.6f, 3.4f, 1.75f, 1.75f, playerTexture));
		}
		break;
	case 3:
		CLIENT_ID = id;
		std::cout << "Client ID set! [ " << id << " ] " << std::endl;
		break;
	}
}

DWORD WINAPI NetLoop(__in LPVOID lpParameter)
{
	ThreadParams* params = (ThreadParams*)lpParameter;

	while (true)
	{
		ENetEvent event;
		while (enet_host_service(params->client, &event, 0) > 0)
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
}

ENetAddress address;
ENetPeer* peer;
ENetHost* client;
ENetEvent event;

class Game : public Augiwne
{
private:
	Window* window;
	Sprite* player;
	Sprite* p2;
	Sprite* ball;
	Sprite* sprWolf;
	Shader* diffuse;
	Camera* cam;
	Texture* wolf[4];
	Renderable2D* players[5];
	float timer;
	float speed = 0.35f;
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
		if (enet_initialize() != 0)
		{
			fprintf(stderr, "An error occurred while initializng ENet!\n");
			exit(EXIT_FAILURE);
		}

		client = enet_host_create(NULL, 1, 1, 0, 0);

		if (client == NULL)
		{
			fprintf(stderr, "An error occurred while trying to create an ENet client!\n");
			exit(EXIT_FAILURE);
		}
		
		enet_address_set_host(&address, "127.0.0.1");
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

		ThreadParams params;
		params.client = client;

		CreateThread(NULL, 0, NetLoop, &params, 0, NULL);
	}
	~Game()
	{
		enet_deinitialize();
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
		
		layer = new Layer(new BatchRenderer2D(), diffuse, ortho);

		const string path = "Data/textures/";
		Texture* textures[] =
		{
			new Texture(path + "background.png"),
			new Texture(path + "Paddle.png"),
			new Texture(path + "Paddle_2.png"),
			new Texture(path + "Ball.png"),
			new Texture(path + "Wall.png")
		};

		playerTexture = new Texture(path + "Paddle.png");
		player = new Sprite(-16.6f, 0, 1.75f, 1.75f, playerTexture);

		p2 = new Sprite(16.6f, 0, 0.24f, 2.5f, textures[2]);
		ball = new Sprite(0, 0, 0.64f, 0.64f, textures[3]);
	
		layer->Add(new Sprite(-18.3f, -16, 16 * 4, 9 * 3, textures[0]));

		for (float x = -16.0f; x < 16.0f; x += 1.0f)
		{
			for (float y = -9.0f; y < 9.0f; y += 1.0f)
			{
				layer->Add(new Sprite(x, y, 1.0f, 1.0f, textures[4]));
			}
		}

		layer->Add(player);
		layer->Add(p2);
		layer->Add(ball);

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

		for (int i = 0; i < layer->GetRenderables().size(); i++) {
			diffuse->Enable();

			Renderable2D* rend = layer->GetRenderables().at(i);

			switch (i)
			{
			case 1:
			{
				Vector3 pos = rend->GetPosition();

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

				rend->SetPosition(pos);
				break;
			}
			case 2:
			{
				Vector3 pos = rend->GetPosition();

				if (window->IsKeyPressed(GLFW_KEY_UP))
				{
					if (pos.y > 6)
						break;

					pos.y += speed * delta;

				}
				else if (window->IsKeyPressed(GLFW_KEY_DOWN))
				{
					if (pos.y < -8.6)
						break;

					pos.y -= speed * delta;
				}

				rend->SetPosition(pos);
				break;
			}
			case 3:
			{
				Vector3 pos = rend->GetPosition();

				if (AABBCollision(layer->GetRenderables().at(1), rend))
				{
					tx = .4f;

					heading = Direction::RIGHT;
				}

				if (AABBCollision(layer->GetRenderables().at(2), rend))
				{
					tx = - .4f;

					int rg = .2f - .1f + 1;

					ty = rand() % rg + .1f;
					heading = Direction::LEFT;
				}

				// Out of range
				if (pos.y > 4 || pos.y < 0)
				{
					ty = -ty / 2;
				}

			//	if (pos.x > 15.5f || pos.x < -15.5f)
			//	{
			//		tx = -tx / 2;
			//	}

				pos.x += tx * delta;
				pos.y += ty * delta;

				rend->SetPosition(pos);

				break;
			}
			}
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

		// - Layer Render
		layer->Render();
	}
};

int main()
{
	Game game;
	game.Start();

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

	return 0;
}