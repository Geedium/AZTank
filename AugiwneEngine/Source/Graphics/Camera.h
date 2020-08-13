#pragma once
#include <Maths\Maths.h>

namespace Augiwne {	namespace Graphics {
	class Camera
	{
	protected:
		Maths::Matrix4 m_ProjectionMatrix, m_ViewMatrix;
		Maths::Vector3 m_Position, m_Rotation, m_FocalPoint;
	public:
		Camera(const Maths::Matrix4& m_ProjectionMatrix);
		virtual void Focus() { }
		virtual void Update() { }
		inline const Maths::Vector3& GetPosition() const { return m_Position; }
		inline void SetPosition(const Maths::Vector3& position) { m_Position = position; }

		inline const Maths::Vector3& GetRotation() const { return m_Rotation; }
		inline void SetRotation(const Maths::Vector3& rotation) { m_Rotation = rotation; }

		inline const Maths::Matrix4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		inline void SetProjectionMatrix(const Maths::Matrix4& projectionMatrix) { m_ProjectionMatrix = projectionMatrix; }

		inline void Translate(const Maths::Vector3& translation) { m_Position += translation; }
		inline void Rotate(const Maths::Vector3& rotation) { m_Rotation += rotation; }

		inline void Translate(float x, float y, float z) { m_Position += Maths::Vector3(x, y, z); }
		inline void Rotate(float x, float y, float z) { m_Rotation += Maths::Vector3(x, y, z); }

		inline const Maths::Vector3& GetFocalPoint() const { return m_FocalPoint; }

		inline const Maths::Matrix4& GetViewMatrix() { return m_ViewMatrix; }

	};
}}
