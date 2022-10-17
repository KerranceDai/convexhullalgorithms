#include "Point.h"

Point::Point() : Point(0, 0) {}

Point::Point(int x, int y) : Point((float)x, (float)y) {}

Point::Point(float x, float y)
{
	this->x = x;
	this->y = y;
}