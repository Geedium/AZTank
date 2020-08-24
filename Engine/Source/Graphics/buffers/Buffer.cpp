#include "System.h"
#include "buffer.h"

namespace Augiwne { namespace Graphics {
	Buffer::Buffer(GLfloat* data, GLsizei count, GLuint componentCount)
		: m_ComponentCount(componentCount)
	{
		glGenBuffers(1, &m_BufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(GLfloat), data, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	Buffer::~Buffer()
	{
		glDeleteBuffers(1, &m_BufferID);
	}
	void Buffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
	}
	void Buffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}}