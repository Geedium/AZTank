#pragma once

#include "..\Renderers\2D\renderable2d.h"
#include "..\Renderers\2D\Renderer2D.h"

namespace Augiwne { namespace Graphics {
	class Layer {
	protected:
		Renderer2D* m_Renderer;
		std::vector<Renderable2D*> m_Renderables;
		Shader* m_Shader;
		Maths::Matrix4 m_ProjectionMatrix;
	public:
		Layer(Renderer2D* renderer, Shader* shader, Maths::Matrix4 matrix);
		virtual ~Layer();
		virtual void Add(Renderable2D* renderable);
		virtual void Remove(Renderable2D* renderable);
		virtual void Render();
		inline const std::vector<Renderable2D*>& GetRenderables() const { return m_Renderables; }
	};
}}