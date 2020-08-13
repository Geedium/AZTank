#pragma once
#include "..\..\StaticSprite.h"
#include <deque>
#include "Renderer2D.h"

namespace Augiwne { namespace Graphics {
		class SimpleRenderer2D : public Renderer2D
		{
		private:
			std::deque<const StaticSprite*> m_RenderQueue;
		public:
			void Submit(const Renderable2D* renderable) override;
			void Flush() override;
		};
}}