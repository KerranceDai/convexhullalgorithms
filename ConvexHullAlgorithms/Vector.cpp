#include "Vector.h"

Vector::Vector()
{
	this->x = 0;
	this->y = 0;
}

Vector::Vector(float x, float y)
{
	this->x = x;
	this->y = y;
}

Vector::Vector(Point start, Point end)
{
	this->start = start;
	this->end = end;
	this->x = end.x - start.x;
	this->y = end.y - start.y;
}

Vector Vector::Normalize()
{
	Vector unit;
	float dx = this->x;
	float dy = this->y;
	float angle = abs(atan(dy / dx));
	float y = sin(angle);
	float x = cos(angle);

	unit.x = dx > 0 ? x : x * -1;
	unit.y = dy > 0 ? y : y * -1;

	return unit;
}

Vector Vector::Perpendicular()
{
	Vector perp;
	perp.x = this->y * -1;
	perp.y = this->x;
	return perp;
}