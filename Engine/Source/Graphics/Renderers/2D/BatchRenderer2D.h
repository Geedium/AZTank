#pragma once

#include <cstddef>
#include "Renderer2D.h"
#include "renderable2d.h"
#include "../../buffers/indexBuffer.h"

namespace Augiwne { namespace Graphics {
#define RENDERER_MAX_SPRITES 60000
#define RENDERER_VERTEX_SIZE sizeof(VertexData)
#define RENDERER_SPRITE_SIZE RENDERER_VERTEX_SIZE * 4
#define RENDERER_BUFFER_SIZE RENDERER_SPRITE_SIZE * RENDERER_MAX_SPRITES
#define RENDERER_INDICES_SIZE RENDERER_MAX_SPRITES * 6
#define RENDERER_MAX_TEXTURES 32

#define SHADER_VERTEX_INDEX 0
#define SHADER_UV_INDEX 1
#define SHADER_TID_INDEX 2
#define SHADER_COLOR_INDEX 3


	class BatchRenderer2D : public Renderer2D
	{
	private:
		GLuint m_Buffer;
		GLuint m_VertexArray;
		IndexBuffer* m_IndexBuffer;
		GLsizei m_IndexBufferCount;
		VertexData* m_DataBuffer;
		std::vector<GLuint> m_TextureSlots;
	public:
		BatchRenderer2D();
		~BatchRenderer2D();
	private:
		void Init();
	public:
		void Begin() override;
		void Submit(const Renderable2D* renderable) override;
		void End() override;
		void Flush() override;
	};
}}