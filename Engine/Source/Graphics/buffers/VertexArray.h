#pragma once
#include <vector>
#include <glew.h>
#include "buffer.h"

namespace Augiwne { namespace Graphics {
	class VertexArray
	{
	private:
		unsigned int m_ArrayID;
		std::vector<Buffer*> m_Buffers;
	public:
		VertexArray();
		~VertexArray();

		void AddBuffer(Buffer* buffer, unsigned int index);
		void Bind() const;
		void Unbind() const;
	};
}}