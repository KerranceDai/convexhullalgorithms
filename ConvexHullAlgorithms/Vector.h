#pragma once

#include "Point.h"
#include <cmath>

struct Vector {
	Point start, end;
	float x, y;

	Vector();
	Vector(float x, float y);
	Vector(Point start, Point end);

	Vector Normalize();
	Vector Perpendicular();
};