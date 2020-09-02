#include "System.h"
#include "Vector3.h"

namespace Augiwne { namespace Maths {
	Vector3::Vector3(const float& x, const float& y, const float& z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
	Vector3& Vector3::Normalize()
	{
		const float len = 1 / sqrt(x * x + y * y + z * z);
		x *= len;
		y *= len;
		z *= len;
		return *this;
	}
	Vector3& Vector3::Add(const Vector3& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}
	Vector3& Vector3::Subtract(const Vector3& other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}
	Vector3& Vector3::Multiply(const Vector3& other)
	{
		x *= other.x;
		y *= other.y;
		z *= other.z;
		return *this;
	}
	Vector3& Vector3::Divide(const Vector3& other)
	{
		x /= other.x;
		y /= other.y;
		z /= other.z;
		return *this;
	}

	Vector3 Vector3::Cross(const Vector3& other) const
	{
		return Vector3(y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x);
	}

	Vector3 Vector3::Lerp(Vector3 a, Vector3 b, float t)
	{
		Vector3 c = b - a;
		c.x *= t;
		c.y *= t;
		c.z *= t;

		return a + c;
	}

	Vector3& operator+(Vector3& left, const Vector3& right) { return left.Add(right); }
	Vector3& operator-(Vector3& left, const Vector3& right) { return left.Subtract(right); }
	Vector3& operator*(Vector3& left, const Vector3& right) { return left.Multiply(right); }
	Vector3& operator/(Vector3& left, const Vector3& right) { return left.Divide(right); }

	Vector3& operator*(Vector3& left, const float right)
	{
		left.x *= right;
		left.y *= right;
		left.z *= right;
		return left;
	}

	Vector3& Vector3::operator+=(const Vector3& other) { return Add(other); }
	Vector3& Vector3::operator-=(const Vector3& other) { return Subtract(other); }
	Vector3& Vector3::operator*=(const Vector3& other) { return Multiply(other); }
	Vector3& Vector3::operator/=(const Vector3& other) { return Divide(other); }

	bool Vector3::operator==(const Vector3& other) { return x == other.x && y == other.y && z == other.z; }
	bool Vector3::operator!=(const Vector3& other) { return !(*this == other); }
}}