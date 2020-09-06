#include "System.h"
#include "Sprite.h"

namespace Augiwne { namespace Graphics {
	Sprite::Sprite(float x, float y, float width, float height, unsigned int color)
		: Renderable2D(Vector3(x, y, 0), Vector2(width, height), color), position(m_Position)
	{

	}
	Sprite::Sprite(float x, float y, float width, float height, Texture* texture)
		: Renderable2D(Vector3(x, y, 0), Vector2(width, height), 0xffffffff), position(m_Position)
	{
		m_Texture = texture;
	}
}}