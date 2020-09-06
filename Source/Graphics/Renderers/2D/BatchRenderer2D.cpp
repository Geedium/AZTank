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

		FT_Error error = FT_Init_FreeType(&library);
		if (error) {
			std::cout << "Error occurred during library initialization" << std::endl;
			exit(0);
		}

		error = FT_New_Face(library, "Bagnard.otf", 0, &face);
		if (error == FT_Err_Unknown_File_Format)
		{
			std::cout << "Font file format could not be read!" << std::endl;
			exit(0);
		}
		else if (error)
		{
			std::cout << "Font loading error!" << std::endl;
			exit(0);
		}

		error = FT_Set_Pixel_Sizes(face, 0, 24);

		/*
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

		for (unsigned char c = 0; c < 128; c++)
		{
			// load character glyph 
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				continue;
			}
			// generate texture
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);
			// set texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			// now store character for later use
			Character character = {
				texture,
				Vector2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				Vector2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};
			Characters.insert(std::pair<char, Character>(c, character));
		}
		*/
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
	void BatchRenderer2D::DrawString(const std::string& text, const Vector3& position, const Vector4& color)
	{
		std::string::const_iterator c;
		Vector3 offset = position;

		float scaleX = 960.0f / 32.0f;
		float scaleY = 540.0f / 10.0f;

		for (c = text.begin(); c != text.end(); c++)
		{
			Character ch = Characters[*c];

			float xpos = offset.x + ch.Bearing.x / scaleX;
			float ypos = offset.y - (ch.Size.y - ch.Bearing.y) / scaleY;

			float w = ch.Size.x / scaleX;
			float h = ch.Size.y / scaleY;

			int r = color.x * 255.0f;
			int g = color.y * 255.0f;
			int b = color.z * 255.0f;
			int a = color.w * 255.0f;

			GLuint c = a << 24 | b << 16 | g << 8 | r;

			const GLint tid = ch.TextureID;
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

			m_DataBuffer->vertex = *m_TransformationBack * Vector3(xpos, ypos, 0);
			m_DataBuffer->uv = Vector2(0, 0);
			m_DataBuffer->tid = ts;
			m_DataBuffer->color = c;
			m_DataBuffer++;

			m_DataBuffer->vertex = *m_TransformationBack * Vector3(xpos, ypos + h, 0);
			m_DataBuffer->uv = Vector2(0, 1);
			m_DataBuffer->tid = ts;
			m_DataBuffer->color = c;
			m_DataBuffer++;

			m_DataBuffer->vertex = *m_TransformationBack * Vector3(xpos + w, ypos + h, 0);
			m_DataBuffer->uv = Vector2(1, 1);
			m_DataBuffer->tid = ts;
			m_DataBuffer->color = c;
			m_DataBuffer++;

			m_DataBuffer->vertex = *m_TransformationBack * Vector3(xpos + w, ypos, 0);
			m_DataBuffer->uv = Vector2(1, 0);
			m_DataBuffer->tid = ts;
			m_DataBuffer->color = c;
			m_DataBuffer++;

			m_IndexBufferCount += 6;

			offset.x += (ch.Advance >> 6) / scaleX;
		}
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