#pragma once
#include <glew.h>

namespace Augiwne {
	namespace Graphics {
		class IndexBuffer
		{
		private:
			GLuint m_BufferID;
			GLuint m_ComponentCount;
		public:
			IndexBuffer(GLushort* data, GLsizei count);
			IndexBuffer(GLuint* data, GLsizei count);
			~IndexBuffer();
			void Bind() const;
			void Unbind() const;

			inline GLuint GetComponentCount() const { return m_ComponentCount; }
		};
	}
}