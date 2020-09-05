#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"

using namespace Augiwne::Maths;

inline float ToRadians(float degrees)
{
	return degrees * ((float)M_PI / 180.0f);
}

inline float distance(Vector2 a, Vector2 b)
{
	Vector2 c(b.x - a.x, b.y - a.y);
	return sqrt(c.x * c.x + c.y * c.y);
}