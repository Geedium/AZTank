#pragma once
#include "Renderers\2D\renderable2d.h"

namespace Augiwne { namespace Graphics {
	class Sprite : public Renderable2D
	{
	public:
		Vector3& position;
	public:
		Sprite(float x, float y, float width, float height, unsigned int color);
		Sprite(float x, float y, float width, float height, Texture* texture);
	};
}}