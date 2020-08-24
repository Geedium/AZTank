#include "System.h"
#include "BatchRenderer2D.h"

namespace Augiwne { namespace Graphics {
	BatchRenderer2D::BatchRenderer2D()
	{
		Init();
	}
	BatchRenderer2D::~BatchRenderer2D()
	{
		delete m_IndexBuffer;
		glDeleteBuffers(1, &m_Buffer);
	}
	void BatchRenderer2D::Init()
	{
		glGenVertexArrays(1, &m_VertexArray);
		glGenBuffers(1, &m_Buffer);

		glBindVertexArray(m_VertexArray);
		glBindBuffer(GL_ARRAY_BUFFER, m_Buffer);
		glBufferData(GL_ARRAY_BUFFER, RENDERER_BUFFER_SIZE, NULL, GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(SHADER_VERTEX_INDEX);
		glEnableVertexAttribArray(SHADER_UV_INDEX);
		glEnableVertexAttribArray(SHADER_TID_INDEX);
		glEnableVertexAttribArray(SHADER_COLOR_INDEX);

		glVertexAttribPointer(SHADER_VERTEX_INDEX, 3, GL_FLOAT, GL_FALSE, RENDERER_VERTEX_SIZE, (const GLvoid*)0);
		glVertexAttribPointer(SHADER_UV_INDEX, 2, GL_FLOAT, GL_FALSE, RENDERER_VERTEX_SIZE, (const GLvoid*)(offsetof(VertexData, VertexData::uv)));
		glVertexAttribPointer(SHADER_TID_INDEX, 1, GL_FLOAT, GL_FALSE, RENDERER_VERTEX_SIZE, (const GLvoid*)(offsetof(VertexData, VertexData::tid)));
		glVertexAttribPointer(SHADER_COLOR_INDEX, 4, GL_UNSIGNED_BYTE, GL_TRUE, RENDERER_VERTEX_SIZE, (const GLvoid*)(offsetof(VertexData, VertexData::color)));

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		GLuint* indices = new GLuint[RENDERER_INDICES_SIZE];
		int offset = 0;
		for (int i = 0; i < RENDERER_INDICES_SIZE; i += 6)
		{
			indices[i] = offset + 0;
			indices[i + 1] = offset + 1;
			indices[i + 2] = offset + 2;
			indices[i + 3] = offset + 2;
			indices[i + 4] = offset + 3;
			indices[i + 5] = offset + 0;
			offset += 4;
		}
		m_IndexBuffer = new IndexBuffer(indices, RENDERER_INDICES_SIZE);
		glBindVertexArray(0);
	}
	void BatchRenderer2D::Begin()
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_Buffer);
		m_DataBuffer = (VertexData*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	}
	void BatchRenderer2D::Submit(const Renderable2D* renderable)
	{
		const Maths::Vector3& pos = renderable->GetPosition();
		const Maths::Vector2& size = renderable->GetSize();
		const unsigned int color = renderable->GetColor();
		const std::vector<Maths::Vector2>& uv = renderable->GetUV();
		const GLint tid = renderable->GetTID();
		float ts = 0.0f;

		if (tid > 0)
		{
			bool found = false;
			for (int i = 0; i < m_TextureSlots.size(); i++)
			{
				if (m_TextureSlots[i] == tid) 
				{
					ts = (float)(i + 1);
					found = true;
					break;
				}
			}
			if (!found)
			{
				if (m_TextureSlots.size() >= RENDERER_MAX_TEXTURES)
				{
					End();
					Flush();
					Begin();
				}
				m_TextureSlots.push_back(tid);
				ts = (float)(m_TextureSlots.size());
			}
		}
		m_DataBuffer->vertex = *m_TransformationBack * pos;
		m_DataBuffer->uv = uv[0];
		m_DataBuffer->tid = ts;
		m_DataBuffer->color = color;
		m_DataBuffer++;

		m_DataBuffer->vertex = *m_TransformationBack * Maths::Vector3(pos.x, pos.y + size.y, pos.z);
		m_DataBuffer->uv = uv[1];
		m_DataBuffer->tid = ts;
		m_DataBuffer->color = color;
		m_DataBuffer++;

		m_DataBuffer->vertex = *m_TransformationBack * Maths::Vector3(pos.x + size.x, pos.y + size.y, pos.z);
		m_DataBuffer->uv = uv[2];
		m_DataBuffer->tid = ts;
		m_DataBuffer->color = color;
		m_DataBuffer++;

		m_DataBuffer->vertex = *m_TransformationBack * Maths::Vector3(pos.x + size.x, pos.y, pos.z);
		m_DataBuffer->uv = uv[3];
		m_DataBuffer->tid = ts;
		m_DataBuffer->color = color;
		m_DataBuffer++;

		m_IndexBufferCount += 6;
	}
	void BatchRenderer2D::Flush()
	{
		for (int i = 0; i < m_TextureSlots.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, m_TextureSlots[i]);
		}
		glBindVertexArray(m_VertexArray);
		m_IndexBuffer->Bind();

		glDrawElements(GL_TRIANGLES, m_IndexBufferCount, GL_UNSIGNED_INT, NULL);

		m_IndexBuffer->Unbind();
		glBindVertexArray(0);
		m_IndexBufferCount = 0;
		m_TextureSlots.clear();
	}
	void BatchRenderer2D::End()
	{
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}}