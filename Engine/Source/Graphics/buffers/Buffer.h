#pragma once
#include <glew.h>

namespace Augiwne { namespace Graphics {
	class Buffer
	{
	private:
		unsigned int m_BufferID;
		unsigned int m_ComponentCount;
	public:
		Buffer(GLfloat* data, GLsizei count, GLuint componentCount);
		~Buffer();
		void Bind() const;
		void Unbind() const;

		inline int GetComponentCount() const { return m_ComponentCount; }
	};
}}