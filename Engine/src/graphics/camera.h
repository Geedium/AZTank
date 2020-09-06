#pragma once
#include <Augiwne.h>

namespace Augiwne
{
	namespace Graphics
	{
		class Camera
		{
		public:
			Camera(const float left, const float right, const float bottom, const float top, const float near, const float far);
		public:
			void Follow(Renderable2D* renderable);
			void Apply(Shader& shader);
		private:
			Matrix4 projection, view;
		};
	}
}