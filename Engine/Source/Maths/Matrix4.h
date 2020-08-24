#pragma once
#include <string>
#include <iostream>

#include "Maths.h"

namespace Augiwne { namespace Maths {
	struct Matrix4 {
		union
		{
			float elements[16];
			Vector4 columns[4];
		};

		Matrix4();
		Matrix4(float diagonal);

		Vector4 gColumn(int index)
		{
			index *= 4;
			return Vector4(elements[index], elements[index + 1], elements[index + 2], elements[index + 3]);
		}

		static Matrix4 Identity();
		static Matrix4 Orthographic(float left, float right, float bottom, float top, float near, float far);
		static Matrix4 Perspective(float fov, float aspectRatio, float near, float far);

		static Matrix4 Translate(const Vector3& translation);
		static Matrix4 Rotate(float angle, const Vector3& axis);
		static Matrix4 Scale(const Vector3& scale);
		Matrix4& Invert();
		Matrix4& Multiply(const Matrix4& other);
		Vector3 Multiply(const Vector3& other) const;
		Vector4 Multiply(const Vector4& other) const;


		friend Matrix4 operator*(Matrix4 left, const Matrix4& right);
		friend Vector3 operator*(const Matrix4& left, const Vector3& right);
		friend Vector4 operator*(const Matrix4& left, const Vector4& right);

		Matrix4& operator*=(const Matrix4& other);
	};
}}