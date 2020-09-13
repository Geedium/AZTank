#include <System.h>
#include <OpenAL\al.h>
#include <OpenAL\alc.h>

#include <vorbis/vorbisfile.h>

#include <enet\enet.h>
#include <Graphics/Quaternion.h>
#include <Augiwne.h>

#include <graphics/camera.h>
#include <node.h>

using namespace Augiwne;
using namespace Graphics;

static int CLIENT_ID = -1;
static std::string SERVER_ADDR;

constexpr enet_uint32	PACKET_PLAYER_UPDATE = 6;

constexpr auto READ_BUFFER_SIZE = 4096;

static int endian = 2;
Texture* bulletTx;

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

Layer* bulletsLayer;

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
bool isConnected;

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

struct BulletData
{
public:
	Vector3 lastPos;
	Vector3 position;
	Sprite* object;
	float angle;
	float radius;
	float timer;
public:
	BulletData(const Vector3 pos, const float angle, const float time)
		: position(pos), angle(angle), lastPos(0,0,0), timer(time)
	{
		object = new Sprite(pos.x, pos.y, 0.16f, 0.16f, bulletTx);

		float left = object->GetPosition().x;
		float right = left + object->GetSize().x;
		float top = object->GetPosition().y;
		float bottom = top + object->GetSize().y;

		float dx = right - left;
		float dy = bottom - top;
		radius = sqrt(dx * dx + dy * dy) / 2;

		bulletsLayer->Add(object);
	}
	void Update(const float delta)
	{
		float theta = ToRadians(angle);

		Vector3 rotation;
		rotation.x = sin(theta);
		rotation.y = cos(theta);

		position += rotation * 0.175f;

		object->SetPosition(position);
	}
};

std::map<int, PlayerData*> player_map;


Texture* playerTexture;
ENetPeer* peer;
bool isDead;

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

std::vector<BulletData*> bullets;

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

		BulletData* data = new BulletData(Vector3(x, y), 0, FLT_MAX);
		bullets.push_back(data);
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
		isConnected = 1;
	}
	else
	{
		std::cout << "Connection to " << SERVER_ADDR << ":4242 failed!" << std::endl;
		enet_peer_reset(peer);
		isConnected = 0;
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
	Window* window; // Render window.
	Camera* camera;
private:
	Layer* ground; // Background rendering layer.
	Layer* foreground; // Foreground rendering layer.
	Layer* players; // Players rendering layer.
private:
	BatchRenderer2D* renderer; // *tmp
	Shader* diffuse; // Diffuse shader.
private:
	Sprite* background; // Background sprite.
	Sprite* player; // Player sprite *tmp.
	Sprite* missle;
	Sprite* explosion;
private:
	int key; // Animation key.
private:
	Texture* backgroundTexture; // Background texture.
	Texture* missleTexture;
	Texture* deadTexture;
	Texture* expo1;
	Texture* expo2;
	Texture* expo3;
	Texture* expo4;
	Node* A[32][18];
private:
	float speed = 0.35f;
	float animFrameDelay = 0;
	bool darkMode;
	bool moved;
	int sequence;
private:
	float angle = 0; // Rotation angle of local player. *tmp
	float nextShoot = 0; // Bullet shoot of local player. *tmp
	float now, last; // Timing (now and last).
	float delta; // Delta time.
private:
	ALCcontext* context; // Audio context.
	ALCdevice* device; // Audio device.
	ALuint source; // Audio source.
	ALuint explodeSource; // ..
	ALuint buffer; // Audio buffer.
	ALuint explodeBuffer;
public:
	const bool AABBCollision(Renderable2D* a, Renderable2D* b) const
	{
		bool cx = a->GetPosition().x + a->GetSize().x >= b->GetPosition().x && b->GetPosition().x + b->GetSize().x >= a->GetPosition().x;
		bool cy = a->GetPosition().y + a->GetSize().y >= b->GetPosition().y && b->GetPosition().y + b->GetSize().y >= a->GetPosition().y;
		return cx && cy;
	}

	const bool RectangleCircle(
		Renderable2D* circle,
		const float radius,
		Renderable2D* rect) const
	{
		Vector3 distance(
			abs(circle->GetPosition().x - rect->GetPosition().x),
			abs(circle->GetPosition().y - rect->GetPosition().y),
			0);

		if (distance.x > (rect->GetSize().x / 2 + radius)) { return false; }
		if (distance.y > (rect->GetSize().y / 2 + radius)) { return false; }
		if (distance.x <= (rect->GetSize().x / 2)) { return true; }
		if (distance.y <= (rect->GetSize().y / 2)) { return true; }

		float x = (distance.x - rect->GetSize().x / 2) * (distance.x - rect->GetSize().x / 2);
		float y = (distance.y - rect->GetSize().y / 2) * (distance.y - rect->GetSize().y / 2);

		float cDist_sq = x + y;

		return (cDist_sq <= (radius * radius));
	}

public:
	Game()
	{
		for (int x = 0; x < 32; x++)
		{
			for (int y = 0; y < 18; y++)
			{
				A[x][y] = new Node();
				std::cout << "[A*] new Node created." << std::endl;
			}
		}

		//AudioManager audioManager;

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
		alGenSources(1, &explodeSource);

		alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
		alGenBuffers((ALuint)1, &buffer);
		alGenBuffers((ALuint)1, &explodeBuffer);

		std::vector<char> oggbuffer;
		std::vector<char> oggexplode;

		int num_channels;
		int freq;

		if (load_ogg("Data/audio/shoot.ogg", &oggbuffer, &num_channels, &freq))
		{
			std::cout << "Failed to load ogg data." << std::endl;
			exit(0);
		}

		if (load_ogg("Data/audio/explode.ogg", &oggexplode, &num_channels, &freq))
		{
			std::cout << "Failed to load explode ogg data." << std::endl;
			exit(0);
		}

		ALenum format;
		if (num_channels == 1)
			format = AL_FORMAT_MONO16;
		else
			format = AL_FORMAT_STEREO16;

		alBufferData(buffer, format, &oggbuffer[0], (ALsizei)oggbuffer.size(), freq);
		alBufferData(explodeBuffer, format, &oggexplode[0], (ALsizei)oggexplode.size(), freq);

		alSourcei(source, AL_BUFFER, buffer);
		alSourcei(explodeSource, AL_BUFFER, explodeBuffer);

		DWORD threadID;
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)NetLoop, NULL, 0, &threadID);
		std::cout << "Network Thread Created: " << threadID << std::endl;
	}
	~Game()
	{
		for (int x = 0; x < 32; x++)
		{
			for (int y = 0; y < 18; y++)
			{
				delete A[x][y];
			}
		}

		alDeleteSources(1, &source);
		alDeleteSources(1, &explodeSource);
		alDeleteBuffers(1, &buffer);
		alDeleteSources(1, &explodeBuffer);
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
		window = CreateRenderWindow("AZ", 960, 540, GetVSync() ); // Create a new render window.
		diffuse = new Shader("Data/Vertex.shader", "Data/Frag.shader"); // Load diffuse shader from file.

		camera = new Camera(-16, 16, -9, 9, -1, 1);

		Matrix4& ortho = Matrix4::Orthographic(-16, 16, -9, 9, -1, 1); // Make orthographic matrix.

		backgroundTexture = new Texture("Data/textures/Background.png"); // Load background texture from file.
		background = new Sprite(-16, -9, 32, 18, backgroundTexture); // Create a new background sprite.

		ground = new Layer(new BatchRenderer2D(), diffuse, ortho); // Create a new background layer.
		ground->Add(background); // Attach background to ground.

		// Missle
		renderer = new BatchRenderer2D();
		missleTexture = new Texture("Data/textures/Missle.png");
		missle = new Sprite(-6.0f, 0.0f, 0.65f, 0.65f, missleTexture);

		deadTexture = new Texture("Data/textures/neutral_256x256.png");

		bulletTx = new Texture("Data/textures/Bullet.png");
		
		expo1 = new Texture("Data/textures/explosion1.png");
		expo2 = new Texture("Data/textures/expo2.png");
		expo3 = new Texture("Data/textures/expo3.png");
		expo4 = new Texture("Data/textures/expo4.png");

		explosion = new Sprite(0, 0, 0.84f, 0.84f, expo1);

		foreground = new Layer(new BatchRenderer2D(), diffuse, ortho); // Create a new foreground layer.

		players = new Layer(new BatchRenderer2D(), diffuse, ortho);
		bulletsLayer = new Layer(new BatchRenderer2D(), diffuse, ortho);

		playerTexture = new Texture("Data/textures/Blue_Tank.png");
		player = new Sprite(-10.6f, 0, 0.5f, 0.5f, playerTexture);
	
		Texture* wallTexture = new Texture("Data/textures/Wall.png");

		int nX = 0;
		int nY = 0;

		for (int i = 0; i < 32; i++)
		{
			float x = (i * 1.0f) - 16;

			for (int j = 0; j < 18; j++)
			{
				float y = (j * 1.0f) - 9;

				bool g = j % 2 == 0 && i % 2 == 0;

				if (g || i == 0 || i == 31 || j == 0 || j == 17)
				{
					foreground->Add(new Sprite(x, y, 1.0f, 1.0f, wallTexture));
					A[i][j]->isObstacle = true;
				}
			}
		}

		for (int x = 0; x < 32; x++)
		{
			for (int y = 0; y < 18; y++)
			{
				if (A[x][y]->isObstacle)
				{
					std::cout << "X ";
				}
				else {
					std::cout << "- ";
				}
			}

			std::cout << std::endl;
		}
	}

	void Tick() override
	{
		std::cout << GetUPS() << " ups, " << GetFPS() << " fps." << std::endl;
	}

	void Update() override
	{
		if (now > last)
		{
			delta = ((now - last) / 0.01f);
			last = now;
		}
		now = m_Watcher->Elapsed();

		if (now > animFrameDelay)
		{
			key++;

			switch (key)
			{
			case 0: explosion->SetTexture(expo1); break;
			case 1: explosion->SetTexture(expo2); break;
			case 2: explosion->SetTexture(expo3); break;
			case 3: explosion->SetTexture(expo4); break;
			}

			if (key > 3)
				key = 0;

			animFrameDelay = now + 0.05f;
		}

		for (int i = 0; i < bullets.size(); i++)
		{
			if (now > bullets[i]->timer)
			{
				alSourceRewind(explodeSource);
				alSourcePlay(explodeSource);
				bullets.erase(bullets.begin() + i);
			}
			else
			{
				bullets[i]->lastPos = bullets[i]->position;
				bullets[i]->Update(delta);
			}
		}

		Vector3 pos = player->GetPosition();
		Vector3 old = pos;
		pos.z = 0;
		old.z = 0;

		bool isColidiing = false;

		float theta = ToRadians(angle);

		Vector3 rotation;
		rotation.x = sin(theta);
		rotation.y = cos(theta);
		rotation.z = 0;

		if (window->IsKeyPressed(GLFW_KEY_SPACE) && now > nextShoot && !isDead)
		{
			alSourceRewind(source);
			alSourcePlay(source);
			nextShoot = now + 0.58888795f;

			if (isConnected)
			{
				Vector3 spawnpos = pos;
				spawnpos += rotation * 1.32f;

				SendBulletPos(spawnpos.x, spawnpos.y);
			}
			else
			{
				Vector3 spawnpos = pos;
				spawnpos += rotation * 1.32f;

				BulletData* data = new BulletData(spawnpos, angle, now + 6);
				bullets.push_back(data);
			}
		}


		if (!isDead)
		{
			if (window->IsKeyPressed(GLFW_KEY_W) && !isColidiing)
			{
				pos += rotation * 0.075f;
				pos.z = 0;
			}
			else if (window->IsKeyPressed(GLFW_KEY_S) && !isColidiing)
			{
				pos -= rotation * 0.075f;
				pos.z = 0;
			}

			if (window->IsKeyPressed(GLFW_KEY_A) && !isColidiing)
			{
				angle -= 1.35f;
				if (angle <= 0)
				{
					angle = 360;
				}
			}
			else if (window->IsKeyPressed(GLFW_KEY_D) && !isColidiing)
			{
				angle += 1.35f;
				if (angle >= 360)
				{
					angle = 0;
				}
			}

			player->SetPosition(pos);
		}
		else
		{
			player->SetPosition(Vector2{ -10.6f, 0.0f });
			isDead = false;
		}

		for (int i = 0; i < bullets.size(); i++)
		{
			if (RectangleCircle(bullets[i]->object, bullets[i]->radius, player))
			{
				player->SetTexture(deadTexture);

				bullets[i]->timer = 0;
				isDead = true;
			}

			for (Renderable2D* otherPlayer : players->GetRenderables())
			{
				if (RectangleCircle(bullets[i]->object, bullets[i]->radius, otherPlayer))
				{

					otherPlayer->SetTexture(deadTexture);
					bullets[i]->timer = 0;
				}
			}
		}

		for (Renderable2D* obj : foreground->GetRenderables())
		{
			if (AABBCollision(obj, player))
			{
				isColidiing = true;
			}

			for (int i = 0; i < bullets.size(); i++)
			{
				if (RectangleCircle(bullets[i]->object, bullets[i]->radius, obj))
				{
					bullets[i]->position = bullets[i]->lastPos;
					if (abs(bullets[i]->angle * -1) == abs(bullets[i]->angle))
					{
						bullets[i]->angle = rand() % 360;
					}
					else
					{
						bullets[i]->angle = bullets[i]->angle * -1;
					}
				}
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
					Vector2 multiply = subtract.Multiply((now - last) / 0.1f);
					Vector2 lerp = value->lastpos + multiply;
					value->object->SetPosition(lerp);

					value->lastpos = lerp;
				}
			}
		}
	}

	void Render() override
	{
		/*
		static double x, y;
		window->GetMousePosition(x, y);

		diffuse->Enable();
		diffuse->SetUniform2f("light_pos",
			Vector2((float)(x * 32 / window->GetWidth() - 16),
				(float)(9 - y * 18 / window->GetHeight())));
		*/

		camera->Follow(player);
		camera->Apply(*diffuse);

		ground->Render(); // Render background rendering layer.
		foreground->Render(); // Render foreground rendering layer.
		bulletsLayer->Render(); // Render bullets rendering layer.
		players->Render(); // Render players rendering layer.

		/* DRAW TEXT
		renderer->Begin();
		renderer->DrawString("0 | 0", Vector3(0, 0, 0), Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		renderer->End();
		renderer->Flush();
		*/

		renderer->Begin();

		Matrix4 model = Matrix4::Identity();
		model *= Matrix4::Translate(Vector3(
			-(player->position.x + player->GetSize().x / 2.0f),
			-(player->position.y + player->GetSize().y / 2.0f),
			-player->position.z));
		model *= Matrix4::Rotate(angle, Vector3(0, 0, 1));
		model *= Matrix4::Translate(Vector3(
			(player->position.x + player->GetSize().x / 2.0f),
			(player->position.y + player->GetSize().y / 2.0f),
			player->position.z));

		renderer->Push(model, true);
		renderer->Submit(player);
		renderer->Pop();

		renderer->Push(Matrix4::Translate(Vector3(
			missle->position.x - 0.08f,
			missle->position.y - 0.55f,
			missle->position.z
		)));

		renderer->Submit(explosion);
		renderer->Pop();

		renderer->Submit(missle);

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