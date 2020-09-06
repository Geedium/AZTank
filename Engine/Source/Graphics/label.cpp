#include "System.h"
#include "label.h"

namespace Augiwne {
	namespace Graphics {
		Label::Label(std::string text, float x, float y, Vector4 color)
			: Renderable2D(), text(text)
		{
			m_Position = Vector3(x, y, 0.0f);
		}

		void Label::Submit(Renderer2D* renderer) const
		{
			
		}
	}
}