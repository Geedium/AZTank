#pragma once

#include <FreeImage.h>
#include <string>
#include <glew.h>
#include "..\Utilities\Image.h"

namespace Augiwne { namespace Graphics {
	class Texture {
	private:
		std::string m_Path;
		GLuint m_TID;
		GLsizei m_Width;
		GLsizei m_Height;
	public:
		Texture(const std::string& path);
		~Texture();
	private:
		GLuint Load();
	public:
		void Bind() const;
		void Unbind() const;

		inline const unsigned int GetID() { return m_TID; }
		inline const unsigned int GetWidth() const { return m_Width; }
		inline const unsigned int GetHeight() const { return m_Height; }
	};
}}