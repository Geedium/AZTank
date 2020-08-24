#pragma once

#include "..\Renderers\2D\renderable2d.h"

namespace Augiwne { namespace Graphics {
	class Group : public Renderable2D
	{
	private:
		std::vector<Renderable2D*> m_Renderables;
		Maths::Matrix4 m_TransformationMatrix;
	public:
		Group(const Maths::Matrix4& transform);
		~Group();
	public:
		void Add(Renderable2D* renderable);
		void Submit(Renderer2D* renderer) const override;
	};
}}