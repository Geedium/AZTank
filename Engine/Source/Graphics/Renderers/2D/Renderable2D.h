#pragma once
#include "..\..\..\maths/Maths.h"
#include "..\..\buffers\buffer.h"
#include "..\..\buffers\indexBuffer.h"
#include "..\..\buffers\vertexArray.h"
#include "Renderer2D.h"
#include "..\..\shader.h"
#include "..\..\Texture.h"

namespace Augiwne { namespace Graphics {
	struct VertexData
	{
		Maths::Vector3 vertex;
		Maths::Vector2 uv;
		float tid;
		unsigned int color;
	};

	class Renderable2D
	{
	protected:
		Maths::Vector3 m_Position;
		Maths::Vector2 m_Size;
		unsigned int m_Color;
		std::vector<Maths::Vector2> m_UV;
		Texture* m_Texture;
	protected:
		Renderable2D() {
			SetUVDefaults();
		}
	private:
		void SetUVDefaults()
		{
			m_UV.push_back(Maths::Vector2(0, 0));
			m_UV.push_back(Maths::Vector2(0, 1));
			m_UV.push_back(Maths::Vector2(1, 1));
			m_UV.push_back(Maths::Vector2(1, 0));
		}
	public:
		Renderable2D(Maths::Vector3 pos, Maths::Vector2 size, unsigned int color)
			: m_Position(pos), m_Size(size), m_Color(color), m_Texture(nullptr) {
			SetUVDefaults();
		}
		virtual ~Renderable2D() {}

		virtual void Submit(Renderer2D* renderer) const
		{
			renderer->Submit(this);
		}
		void SetColor(unsigned int color) { 
			m_Color = color;
		}
		void SetTexture(Texture* texture) {
			m_Texture = texture;
		}
		void SetPosition(Vector3& pos) {
			m_Position = pos;
		}
		void SetPosition(Vector2& pos) {
			m_Position = Vector3(pos.x, pos.y, 0);
		}
		void SwapHorizontal(bool swap) {
			if (swap)
			{
				m_UV.at(0) = Vector2(0, 0);
				m_UV.at(1) = Vector2(0, 1);
				m_UV.at(2) = Vector2(-1, 1);
				m_UV.at(3) = Vector2(-1, 0);
			}
			else {
				m_UV.at(0) = Vector2(0, 0);
				m_UV.at(1) = Vector2(0, 1);
				m_UV.at(2) = Vector2(1, 1);
				m_UV.at(3) = Vector2(1, 0);
			}
		}
		void SetColor(Vector4& color) {
			int r = (int)(color.x * 255);
			int g = (int)(color.y * 255);
			int b = (int)(color.z * 255);
			int a = (int)(color.w * 255);
			m_Color = a << 24 | b << 16 | g << 8 | r;
		}

		bool AABB(const Renderable2D& other) const
		{
			const bool x = m_Position.x + m_Size.x >= other.GetPosition().x && other.GetPosition().x + other.GetSize().x >= m_Position.x;
			const bool y = m_Position.y + m_Size.y >= other.GetPosition().y && other.GetPosition().y + other.GetSize().y >= m_Position.y;
			return x && y;
		}

		inline const Maths::Vector2& GetSize() const { return m_Size; }
		inline const Maths::Vector3& GetPosition() const { return m_Position; }
		inline const unsigned int GetColor() const { return m_Color; }
		inline const std::vector<Maths::Vector2>& GetUV() const { return m_UV; }
		inline const unsigned int GetTID() const { return m_Texture == nullptr ? 0 : m_Texture->GetID(); }
	};
}}