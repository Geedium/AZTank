#include "System.h"
#include "Quaternion.h"

namespace Augiwne
{
	namespace Graphics
	{
		Quaternion::Quaternion(const Vector3 pos)
		{
			x = pos.x;
			y = pos.y;
			z = pos.z;
			w = 0;
		}

		Quaternion::Quaternion(const Vector3 axis, const float theta)
		{
			const float h = theta / 2;
			const float s = sin(h);

			x = axis.x * s;
			y = axis.y * s;
			z = axis.z * s;
			w = cos(h);

			const float n = sqrt(x * x + y * y + z * z + w * w);

			x /= n;
			y /= n;
			z /= n;
			w /= n;
		}

		Quaternion& Quaternion::Multiply(Quaternion& other)
		{
			x = (w * other.x) + (x * other.w) + (y * other.z) - (z * other.y);
			y = (w * other.y) - (x * other.z) + (y * other.w) + (z * other.x);
			z = (w * other.z) + (x * other.y) - (y * other.x) + (z * other.w);
			w = (w * other.w) - (x * other.x) - (y * other.y) - (z * other.z);
			return *this;
		}

		Matrix4 Quaternion::toMatrix()
		{
			Matrix4 A;

			A.columns[0].x = 1 - 2 * (y * y) - 2 * (z * z);
			A.columns[0].y = 2 * x * y + 2 * z * w;
			A.columns[0].z = 2 * x * z - 2 * y * w;

			A.columns[1].x = 2 * x * y - 2 * z * w;
			A.columns[1].y = 1 - 2 * (x * x) - 2 * (z * z);
			A.columns[1].z = 2 * y * z + 2 * x * w;

			A.columns[2].x = 2 * x * z + 2 * y * w;
			A.columns[2].y = 2 * y * z - 2 * x * w;
			A.columns[2].z = 1 - 2 * (x * x) - 2 * (y * y);

			return A;

			/*
			Matrix4 A;
			A.elements[0] = w;
			A.elements[1] = z;
			A.elements[2] = -y;
			A.elements[3] = x;
			A.elements[4] = -z;
			A.elements[5] = w;
			A.elements[6] = x;
			A.elements[7] = y;
			A.elements[8] = y;
			A.elements[9] = -x;
			A.elements[10] = w;
			A.elements[11] = z;
			A.elements[12] = -x;
			A.elements[13] = -y;
			A.elements[14] = -z;
			A.elements[15] = w;

			Matrix4 B;
			B.elements[0] = w;
			B.elements[1] = z;
			B.elements[2] = -y;
			B.elements[3] = -x;
			B.elements[4] = -z;
			B.elements[5] = w;
			B.elements[6] = x;
			B.elements[7] = -y;
			B.elements[8] = y;
			B.elements[9] = -x;
			B.elements[10] = w;
			B.elements[11] = -z;
			B.elements[12] = x;
			B.elements[13] = y;
			B.elements[14] = z;
			B.elements[15] = w;

			return A * B;*/
		}
	}
}