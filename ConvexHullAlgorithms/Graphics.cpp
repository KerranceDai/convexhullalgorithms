#include "Graphics.h"

Graphics::Graphics()
{
	factory = NULL;
	render = NULL;
	brush = NULL;
}

Graphics::~Graphics()
{
	if (factory) factory->Release();
	if (render) render->Release();
	if (brush) brush->Release();
}

bool Graphics::Init(HWND hwnd)
{
	HRESULT res = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);
	if (res != S_OK) return false;

	RECT rect;
	GetClientRect(hwnd, &rect);

	res = factory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(rect.right, rect.bottom)),
		&render
	);
	if (res != S_OK) return false;

	res = render->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 0), &brush);
	if (res != S_OK) return false;

	return true;
}

void Graphics::Clear(float r, float g, float b)
{
	render->Clear(D2D1::ColorF(r, g, b));
}

void Graphics::SetBrush(Color color)
{
	brush->SetColor(D2D1::ColorF(color.r, color.g, color.b, color.a));
}

void Graphics::DrawLine(float x1, float y1, float x2, float y2, Color color, float stroke)
{
	SetBrush(color);
	render->DrawLine(D2D1::Point2F(x1, y1), D2D1::Point2F(x2, y2), brush, stroke);
}

void Graphics::DrawCircle(float x, float y, float radius, Color color, float stroke)
{
	SetBrush(color);
	render->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius), brush, stroke);
}

void Graphics::FillCircle(float x, float y, float radius, Color color)
{
	SetBrush(color);
	render->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius), brush);
}

void Graphics::DrawRect(float x1, float y1, float x2, float y2, Color color, float stroke)
{
	SetBrush(color);
	render->DrawRectangle(D2D1::RectF(x1, y1, x2, y2), brush, stroke);
}

void Graphics::FillRect(float x1, float y1, float x2, float y2, Color color)
{
	SetBrush(color);
	render->FillRectangle(D2D1::RectF(x1, y1, x2, y2), brush);
}