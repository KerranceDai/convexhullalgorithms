#pragma once

#include <windows.h>
#include <d2d1.h>
#include "Color.h"

class Graphics {
public:
	ID2D1Factory* factory;
	ID2D1HwndRenderTarget* render;
	ID2D1SolidColorBrush* brush;

	Graphics();
	~Graphics();

	bool Init(HWND hwnd);

	void BeginDraw() { render->BeginDraw(); }
	void EndDraw() { render->EndDraw(); }

	void Clear(float r, float g, float b);

	void SetBrush(Color color);
	void DrawLine(float x1, float y1, float x2, float y2, Color color, float stroke);
	void DrawCircle(float x, float y, float radius, Color color, float stroke);
	void FillCircle(float x, float y, float radius, Color color);
	void DrawRect(float x1, float y1, float x2, float y2, Color color, float stroke);
	void FillRect(float x1, float y1, float x2, float y2, Color color);
};