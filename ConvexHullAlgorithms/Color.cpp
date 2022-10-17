#include "Color.h"

Color::Color() : Color(0, 0, 0, 255) {}

Color::Color(int r, int g, int b) : Color(r, g, b, 255) {}

Color::Color(int r, int g, int b, int a)
{
	this->r = r / 255.0f;
	this->g = g / 255.0f;
	this->b = b / 255.0f;
	this->a = a / 255.0f;
}