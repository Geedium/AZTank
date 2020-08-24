#pragma once
#include "Camera.h"

namespace Augiwne { namespace Graphics {
	class OrthographicCamera : public Camera
	{
	private:

	public:
		OrthographicCamera(const Maths::Matrix4& projectionMatrix);
		~OrthographicCamera();
		void Focus() override;
		void Update() override;
	};
}}