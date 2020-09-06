#pragma once

#include "Renderers/2D/Renderable2D.h"

namespace Augiwne {
	namespace Graphics {
		class Label : public Renderable2D
		{
		private:
			std::string text;
		public:
			Label(std::string text, float x, float y, Vector4 color);
		public:
			void Submit(Renderer2D* renderer) const;
		};
	}
}