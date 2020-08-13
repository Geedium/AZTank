#pragma once
#include "../Maths/Maths.h"
#include "Renderers\2D\renderable2d.h"

namespace Augiwne { namespace Graphics {
	class StaticSprite : public Renderable2D
	{
	private:
		VertexArray* m_VertexArray;
		IndexBuffer* m_IndexBuffer;
		Shader& m_Shader;
	public:
		StaticSprite(float x, float y, float width, float height, const Maths::Vector4& color, Shader& shader);
		~StaticSprite();

		inline const VertexArray* GetVertexArray() const { return m_VertexArray; }
		inline const IndexBuffer* GetIndexBuffer() const { return m_IndexBuffer; }
		inline Shader& GetShader() const { return m_Shader; }
	};
}}