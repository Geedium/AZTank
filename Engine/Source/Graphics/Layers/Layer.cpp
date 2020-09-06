#include "System.h"
#include "Layer.h"

namespace Augiwne { namespace Graphics {
	Layer::Layer(Renderer2D* renderer, Shader* shader, Maths::Matrix4 matrix)
		: m_Renderer(renderer), m_Shader(shader), m_ProjectionMatrix(matrix)
	{
		m_Shader->Enable();
		m_Shader->SetUniformMatrix4("pr_matrix", m_ProjectionMatrix);
		GLint texIDs[32] =
		{
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
			11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
			21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
			31
		};
		m_Shader->SetUniform1iv("textures", texIDs, 32);
		m_Shader->Disable();
	}
	Layer::~Layer()
	{
		delete m_Shader;
		delete m_Renderer;
		for (unsigned int i = 0; i < m_Renderables.size(); i++)
			delete m_Renderables[i];
	}
	void Layer::Add(Renderable2D* renderable)
	{
		m_Renderables.push_back(renderable);
	}
	void Layer::Remove(Renderable2D* renderable)
	{
		for (int i = 0; i < m_Renderables.size(); i++)
		{
			if (renderable == m_Renderables[i])
			{
				m_Renderables.erase(m_Renderables.begin() + i);
				delete m_Renderables[i];
			}
		}
	}
	void Layer::Render()
	{
		m_Shader->Enable();
		m_Renderer->Begin();
		for (const Renderable2D* renderable : m_Renderables)
		{
			if (renderable != nullptr)
				renderable->Submit(m_Renderer);
		}
		m_Renderer->End();
		m_Renderer->Flush();
	}
}}