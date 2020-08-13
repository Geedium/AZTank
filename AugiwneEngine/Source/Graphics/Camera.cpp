#include "System.h"
#include "Camera.h"

namespace Augiwne { namespace Graphics {
	Camera::Camera(const Maths::Matrix4& projection)
		: m_ProjectionMatrix(projection)
	{
		m_ViewMatrix = Maths::Matrix4::Identity();
		m_Position = Maths::Vector3();
		m_Rotation = Maths::Vector3();
	}
}}