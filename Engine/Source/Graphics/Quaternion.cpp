#include "System.h"
#include "Quaternion.h"

namespace Augiwne
{
	namespace Graphics
	{
		Quaternion::Quaternion(const Vector3 axis, const float theta)
		{
			const float h = theta / 2;
			const float s = sin(h);

			x = axis.x * s;
			y = axis.y * s;
			z = axis.z * s;

			//Angle should be in radians
			w = cos(h);
		}

		Matrix4 Quaternion::toMatrix(Matrix4& to)
		{
			Matrix4 B;

			to.columns[0].x = w;	to.columns[0].y = z;	to.columns[0].z = -y;	to.columns[0].w = x;
			to.columns[1].x = -z;	to.columns[1].y = w;	to.columns[1].z = x;	to.columns[1].w = y;
			to.columns[2].x = y;	to.columns[2].y = -x;	to.columns[2].z = w;	to.columns[2].w = z;
			to.columns[3].x = -x;	to.columns[3].y = -y;	to.columns[3].z = -z;	to.columns[3].w = w;

			B.columns[0].x = w;		B.columns[0].y = z;		B.columns[0].z = -y;	B.columns[0].w = -x;
			B.columns[1].x = -z;	B.columns[1].y = w;		B.columns[1].z = x;		B.columns[1].w = -y;
			B.columns[2].x = y;		B.columns[2].y = -x;	B.columns[2].z = w;		B.columns[2].w = -z;
			B.columns[3].x = x;		B.columns[3].y = y;		B.columns[3].z = z;		B.columns[3].w = w;

			return to.Multiply(B);
		}
	}
}