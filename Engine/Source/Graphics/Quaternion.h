#pragma once
#include <Maths\Vector3.h>
#include <Maths\Matrix4.h>

namespace Augiwne
{
	namespace Graphics
	{
		using namespace Augiwne::Maths;

		class Quaternion
		{
		public:
			Quaternion(const Vector3 pos);
			Quaternion(const Vector3 axis, const float theta);
		public:
			Quaternion& Multiply(Quaternion& other);
			const Vector4 Get()
			{
				return Vector4(x, y, z, w);
			}
			Matrix4 toMatrix();
		public:
			float x, y, z, w;
		};
	}
}