#pragma once
#include "Graphics/Sprite.h"
#include <Graphics\Texture.h>

class Bullet : public Augiwne::Graphics::Sprite
{
public:
	Bullet(const float x, const float y, Augiwne::Graphics::Texture* tx);
public:
	void Update();
};