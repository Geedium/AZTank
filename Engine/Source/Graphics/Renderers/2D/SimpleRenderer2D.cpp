#include "System.h"
#include "SimpleRenderer2D.h"

namespace Augiwne { namespace Graphics {
	void SimpleRenderer2D::Submit(const Renderable2D* renderable)
	{
		m_RenderQueue.push_back((StaticSprite*)renderable);
	}
	void SimpleRenderer2D::Flush()
	{
		while (!m_RenderQueue.empty())
		{
			const StaticSprite* sprite = m_RenderQueue.front();
			sprite->GetVertexArray()->Bind();
			sprite->GetIndexBuffer()->Bind();

			sprite->GetShader().SetUniformMatrix4("ml_matrix", Maths::Matrix4::Translate(sprite->GetPosition()));
			glDrawElements(GL_TRIANGLES, sprite->GetIndexBuffer()->GetComponentCount(), GL_UNSIGNED_SHORT, nullptr);

			sprite->GetIndexBuffer()->Unbind();
			sprite->GetVertexArray()->Unbind();

			m_RenderQueue.pop_front();
		}
	}
}}