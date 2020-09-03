#include <OpenAL\al.h>
#include <OpenAL\alc.h>

#include <vorbis/vorbisfile.h>

#include <enet\enet.h>
#include <Graphics/Quaternion.h>
#include <Augiwne.h>

using namespace Augiwne;
using namespace Graphics;

static int CLIENT_ID = -1;
static std::string SERVER_ADDR;

constexpr enet_uint32	PACKET_PLAYER_UPDATE = 6;

constexpr auto READ_BUFFER_SIZE = 4096;

static int endian = 2;

int load_ogg(const char* file_path, std::vector<char>* buffer, int* num_channels, int* freq)
{
	char readbuf[READ_BUFFER_SIZE];
	FILE* fp;
#ifdef __APPLE__
	char bundle_path[512];
	strcpy(bundle_path, file_path);
	char* p = bundle_path;
	while (*p != '.' && *p != 0)
		p++;
	*p = 0;
	NSString* path = [[NSBundle mainBundle]pathForResource:[NSString stringWithCString : bundle_path encoding : NSUTF8StringEncoding] ofType : @"ogg"];
	strcpy(bundle_path, path.UTF8String);
	fp = fopen(bundle_path, "rb");
#else
	fp = fopen(file_path, "rb");
#endif
	if (fp == NULL)
	{
		std::cout << "Load ogg failed - File not found." << std::endl;
		return 1;
	}
	if (endian == 2)
	{
		uint32_t num = 0x01010000;
		uint8_t* p = (uint8_t*)&num;
		endian = *p;
	}
	OggVorbis_File ogg_file;
	vorbis_info* info;
	ov_open(fp, &ogg_file, NULL, 0);
	info = ov_info(&ogg_file, -1);
	*num_channels = info->channels;
	*freq = (int)info->rate;
	int bit_stream;
	int bytes;
	while (1)
	{
		bytes = ov_read(&ogg_file, readbuf, READ_BUFFER_SIZE, endian, 2, 1, &bit_stream);
		if (bytes > 0)
		{
			int starting_index = buffer->size();
			buffer->resize(buffer->size() + bytes);
			memcpy(&(*buffer)[starting_index], readbuf, bytes);
		}
		else
		{
			if (bytes < 0)
			{
				std::cout << "read error." << std::endl;
			}
			break;
		}
	}
	ov_clear(&ogg_file);
	return 0;
}

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

Layer* bullets;
Texture* bulletTx;

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
	case 4U:
		std::cout << "Spawning a bullet!" << std::endl;
		bullets->Add(new Sprite(x, y, 0.32f, 0.32f, bulletTx));
		break;
	}
}

DWORD WINAPI NetLoop(__in LPVOID lpParameter)
{
	if (enet_initialize() != 0)
	{
		//fprintf(stderr, "An error occurred while initializng ENet!\n");
		exit(EXIT_FAILURE);
	}
	atexit(enet_deinitialize);

	ENetHost* client = enet_host_create(NULL, 1, 1, 0, 0);

	ENetAddress address;
	ENetEvent event;

	if (client == NULL)
	{
		//fprintf(stderr, "An error occurred while trying to create an ENet client!\n");
		exit(EXIT_FAILURE);
	}

	enet_address_set_host(&address, SERVER_ADDR.c_str() );
	address.port = 4242;

	peer = enet_host_connect(client, &address, 1, NULL);
	if (peer == NULL)
	{
		// fprintf(stderr, "No available peers for initiating an ENet connection!\n");
		exit(EXIT_FAILURE);
	}

	if (enet_host_service(client, &event, 5000) > 0 &&
		event.type == ENET_EVENT_TYPE_CONNECT)
	{
		std::cout << "Connection to " << SERVER_ADDR << ":4242 succeeded!" << std::endl;
	}
	else
	{
		std::cout << "Connection to " << SERVER_ADDR << ":4242 failed!" << std::endl;
		enet_peer_reset(peer);
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

void SendBulletPos(const float x, const float y)
{
	std::string packet = "10|";

	const std::string _x = std::to_string(x); // 4 bytes.
	const std::string _y = std::to_string(y); // 4 bytes.

	packet.append(_x).append("|").append(_y);

	SendPacket(peer, packet.c_str());
}

class Game : public Augiwne
{
private:
	Window* window;
private:
	Layer* foreground;
	Layer* players;
private:
	BatchRenderer2D* renderer;
private:
	Sprite* player;
	Sprite* missle;
	Sprite* sprWolf;
	Shader* diffuse;
	Shader* glyphs;
	Shader* playerShader;
	Shader* rot;
	Camera* cam;
	Vector3 newDirectionVector;
	Texture* missleTexture;
	float timer;
	float speed = 0.35f;
	bool darkMode;
	bool moved;
	int sequence;
private: // Sound manager
	ALCcontext* context;
	ALCdevice* device;
	ALuint source;
	ALuint buffer;
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
		device = alcOpenDevice(NULL);
		if (!device)
		{
			std::cout << "FAILED TO CREATE AUDIO DEVICE!" << std::endl;
		}

		context = alcCreateContext(device, NULL);
		if (!alcMakeContextCurrent(context))
		{
			std::cout << "Failed to make context current." << std::endl;
		}

		alGenSources(1, &source);
		//alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
		alSource3f(source, AL_POSITION, 0, 0, 0);
		alSource3f(source, AL_VELOCITY, 0, 0, 0);
		alGenBuffers((ALuint)1, &buffer);
		std::vector<char> oggbuffer;
		int num_channels;
		int freq;
		if (load_ogg("Data/audio/shoot.ogg", &oggbuffer, &num_channels, &freq))
		{
			std::cout << "Failed to load ogg data." << std::endl;
		}

		ALenum format;
		if (num_channels == 1)
			format = AL_FORMAT_MONO16;
		else
			format = AL_FORMAT_STEREO16;
		printf("oggbuffer size=%d, freq=%d\n", (int)oggbuffer.size(), freq);
		alBufferData(buffer, format, &oggbuffer[0], (ALsizei)oggbuffer.size(), freq);
		alSourcei(source, AL_BUFFER, buffer);

		DWORD threadID;
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)NetLoop, NULL, 0, &threadID);
		std::cout << "Network Thread Created: " << threadID << std::endl;
	}
	~Game()
	{
		alDeleteSources(1, &source);
		alDeleteBuffers(1, &buffer);
		device = alcGetContextsDevice(context);
		alcMakeContextCurrent(NULL);
		alcDestroyContext(context);
		alcCloseDevice(device);
	}
	int GetVSync() {
		ifstream file("settings.ini");
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
				else if (name == "ip")
				{
					SERVER_ADDR = value;
				}
				else if (name == "dark_mode")
				{
					darkMode = value == "true" ? true : false;
				}
			}
			file.close();
		}
		return vsyncInterval;
	}
	void Init() override
	{
		window = CreateRenderWindow("AZ", 960, 540, GetVSync());
		diffuse = new Shader("Data/Vertex.shader", darkMode ? "Data/FragDark.shader" : "Data/Frag.shader");

		glyphs = new Shader("Data/GlyphsVertex.shader", "Data/GlyphsFrag.shader");

		playerShader = diffuse;

		Matrix4& ortho = Matrix4::Orthographic(-16, 16, -9, 9, -1, 1);
	
		// Missle
		renderer = new BatchRenderer2D();
		rot = new Shader("Data/Vertex.shader", "Data/Frag.shader");
		missleTexture = new Texture("Data/textures/Missle.png");
		missle = new Sprite(-6.0f, 0.0f, 0.8f, 1.0f, missleTexture);

		bulletTx = new Texture("Data/textures/Bullet.png");

		foreground = new Layer(new BatchRenderer2D(), diffuse, ortho);
		players = new Layer(new BatchRenderer2D(), diffuse, ortho);
		bullets = new Layer(new BatchRenderer2D(), diffuse, ortho);

		playerTexture = new Texture("Data/textures/Paddle.png");
		player = new Sprite(-10.6f, 0, 0.95f, 0.95f, playerTexture);
	
		Texture* wallTexture = new Texture("Data/textures/Wall.png");

		for (float x = -16.0f; x < 16.0f; x += 1.0f)
		{
			for (float y = -9.0f; y < 9.0f; y += 1.0f)
			{
				// Bottom Left to Right
				if (
					(y == -9.0f) ||
					(y == 8.0f) ||
					(x == -16.0f) ||
					(x == 15.0f)
					)
				{
					foreground->Add(new Sprite(x, y, 1.0f, 1.0f, wallTexture));
				}
				else {
					auto p = (int)x % 1 == 0;
					auto j = (int)y % 5 != 1;
					if (p > rand() % 4 && j > rand() % 4 )
					{
						foreground->Add(new Sprite(x, y, 1.0f, 1.0f, wallTexture));
					}
				}
			}
		}
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
	float angle = 0;
	float nextShoot = 0;

	void Update() override
	{
		if (now > last)
		{
			delta = ((now - last) / 0.01f);
			last = now;
		}
		now = m_Watcher->Elapsed();

		for (Renderable2D* bullet : bullets->GetRenderables() )
		{
			Vector3 pos = bullet->GetPosition();
			pos.y += speed * delta;

			bullet->SetPosition(pos);
		}

		//rot->SetUniformMatrix4("ml_matrix", Matrix4::Rotate(angle, Vector3(1, 1, 1)));

		/*

			BEGIN LOCAL_PLAYER

			*/
		Vector3 pos = player->GetPosition();
		Vector3 old = pos;
		bool isColidiing = false;

		float theta = ToRadians(angle);

		Vector3 rotation;
		rotation.x = sin(theta);
		rotation.y = cos(theta);

		if (window->IsKeyPressed(GLFW_KEY_W) && !isColidiing)
		{
			pos += rotation * 0.1f;
		}
		else if (window->IsKeyPressed(GLFW_KEY_S) && !isColidiing)
		{
			pos -= rotation * 0.1f;
		}

		if (window->IsKeyPressed(GLFW_KEY_A) && !isColidiing)
		{
			angle -= speed * 5.0f;
			if (angle <= 0)
			{
				angle = 360;
			}
		}
		else if (window->IsKeyPressed(GLFW_KEY_D) && !isColidiing)
		{
			angle += speed * 5.0f;
			if (angle >= 360)
			{
				angle = 0;
			}
		}
		
		if (window->IsKeyPressed(GLFW_KEY_SPACE) && now > nextShoot)
		{
			alSourceRewind(source);
			alSourcePlay(source);
			nextShoot = now + 0.58888795f;

			SendBulletPos(pos.x, pos.y);
		}

		player->SetPosition(pos);

		for (Renderable2D* obj : foreground->GetRenderables())
		{
			if (AABBCollision(obj, player))
			{
				isColidiing = true;
			}
		}

		if (isColidiing)
		{
			player->SetPosition(old);
		}

		SendPosPacket(pos.x, pos.y);

		for (auto& [key, value] : player_map)
		{
			if (!value->created)
			{
				value->created = true;

				value->object = new Sprite(-16.6f, 0.0f, 0.95f, 0.95f, playerTexture);
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
		;

		// - Layers
		bullets->Render();
		foreground->Render();
		players->Render();

		renderer->Begin();
		renderer->DrawString("0 | 0", Vector3(0, 0, 0), Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		renderer->End();
		renderer->Flush();

		rot->Enable();
		rot->SetUniformMatrix4("pr_matrix", Matrix4::Orthographic(-16.0f, 16.0f, -9.0f, 9.0f, -1.0f, 1.0f));
		rot->SetUniformMatrix4("vw_matrix", Matrix4::Identity());
		rot->SetUniformMatrix4("ml_matrix", Matrix4::Identity());

		renderer->Begin();
		renderer->Submit(missle);
		renderer->End();
		renderer->Flush();

		Matrix4 model = Matrix4::Identity();

		model *= Matrix4::Translate(Vector3(
			-(player->position.x + player->GetSize().x / 2.0f),
			-(player->position.y + player->GetSize().y / 2.0f),
			-player->position.z
		));

		model *= Matrix4::Rotate(0, Vector3(1, 0, 0));
		model *= Matrix4::Rotate(0, Vector3(0, 1, 0));
		model *= Matrix4::Rotate(angle, Vector3(0, 0, 1));

		model *= Matrix4::Translate(Vector3(
			(player->position.x + player->GetSize().x / 2.0f),
			(player->position.y + player->GetSize().y / 2.0f),
			player->position.z
		));

		rot->SetUniformMatrix4("ml_matrix", model);

		renderer->Begin();
		renderer->Submit(player);
		renderer->End();
		renderer->Flush();
	}
};

int main()
{
	Game game;
	game.Start();
	return 0;
}