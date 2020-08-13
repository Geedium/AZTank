#pragma once

namespace Augiwne { namespace Maths {
	struct Vector3 {
		float x, y, z;

		Vector3() = default;
		Vector3(const float& x, const float& y, const float& z);

		Vector3& Add(const Vector3& other);
		Vector3& Subtract(const Vector3& other);
		Vector3& Multiply(const Vector3& other);
		Vector3& Divide(const Vector3& other);
		
		static Vector3 Lerp(Vector3 start, Vector3 end, float amount);

		friend Vector3& operator+(Vector3& left, const Vector3& right);
		friend Vector3& operator-(Vector3& left, const Vector3& right);
		friend Vector3& operator*(Vector3& left, const Vector3& right);
		friend Vector3& operator/(Vector3& left, const Vector3& right);

		Vector3& operator+=(const Vector3& other);
		Vector3& operator-=(const Vector3& other);
		Vector3& operator*=(const Vector3& other);
		Vector3& operator/=(const Vector3& other);

		bool operator==(const Vector3& other);
		bool operator!=(const Vector3& other);
	};
}}