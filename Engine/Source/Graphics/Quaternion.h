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
			Quaternion(const Vector3 axis, const float theta);
			Matrix4 toMatrix(Matrix4& to);
		public:
			float x, y, z, w;
		};
	}
}