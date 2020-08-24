#pragma once
#include <glew.h>
#include <vector>

#include "../../../maths/Maths.h"

namespace Augiwne { namespace Graphics {
	class Renderable2D;
	class Renderer2D
	{
	protected:
		std::vector<Maths::Matrix4> m_TransformationStack;
		const Maths::Matrix4* m_TransformationBack;
	protected:
		Renderer2D() {
			m_TransformationStack.push_back(Maths::Matrix4::Identity());
			m_TransformationBack = &m_TransformationStack.back();
		}
	public:
		void Push(Maths::Matrix4 matrix, bool override = false) {
			if (override) m_TransformationStack.push_back(matrix);
			else m_TransformationStack.push_back(m_TransformationStack.back() * matrix);
			m_TransformationBack = &m_TransformationStack.back();
		}
		void Pop() {
			if (m_TransformationStack.size() > 1)
				m_TransformationStack.pop_back();
			m_TransformationBack = &m_TransformationStack.back();
		}
		virtual void Begin() {}
		virtual void Submit(const Renderable2D* renderable) = 0;
		virtual void End() {}
		virtual void Flush() = 0;
	};
}}