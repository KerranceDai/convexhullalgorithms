#include <windows.h>
#pragma comment(lib, "d2d1")

#include <vector>
#include <time.h>
#include "Graphics.h"
#include "Color.h"
#include "Point.h"
#include "ConvexHull.h"
using namespace std;

// variables
Graphics* graphics;

int screenWidth = 1024;
int screenHeight = 600;

const int uiSplit = 300;
int originX = uiSplit + (screenWidth - uiSplit) / 2;
int originY = screenHeight / 2;

const int defaultGridSize = 24;
int gridSize = defaultGridSize;

// variables for generating random points
const int gridMargin = 2;
int maxX = (screenWidth - originX) / defaultGridSize - gridMargin;
int maxY = (screenHeight - originY) / defaultGridSize - gridMargin;
const int pointRadius = 10;

const float zoomScale = 0.1f;
float zoom = 1;
int offsetX = 0;
int offsetY = 0;

const int buttonMargin = 40;
int selectedButton = -1;
HWND buttons[5];

POINT mousePos;
bool draggingGrid = false;
bool showGrid = true;

Point* draggingPoint = nullptr;
Point* cursorPoint = nullptr;
int pointInHull = 0;
vector<Point>* draggingHull = nullptr;

vector<Point> hull1;
vector<Point> hull1Draw;

vector<Point> hull2;
vector<Point> hull2Draw;

vector<Point> resultHull;

Point testPoint;

float Random(float min, float max)
{
	return ((float)rand() / (float)RAND_MAX) * (max - min) + min;
}

Point PointToGrid(Point p)
{
	float x = originX + offsetX + p.x * gridSize;
	float y = originY + offsetY + p.y * gridSize * -1;
	return Point(x, y);
}

Point GridToPoint(Point p)
{
	float x = (p.x - offsetX - originX) / gridSize;
	float y = (p.y - offsetY - originY) / gridSize * -1;

	return Point(x, y);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_COMMAND:
	{
		offsetX = 0;
		offsetY = 0;
		gridSize = defaultGridSize;
		zoom = 1;

		hull1.clear();
		hull2.clear();
		resultHull.clear();

		selectedButton = wParam;
		switch (wParam)
		{
		case 2: // Quickhull
		case 3: // Point Convex Hull
		{
			showGrid = false;
			
			for (int i = 0; i < 15; i++)
			{
				Point p = Point(Random(-maxX, maxX), Random(-maxY, maxY));
				hull1.push_back(p);
			}

			hull1Draw = Quickhull(&hull1);

			testPoint = Point(Random(-maxX, maxX), Random(-maxY, maxY));

			break;
		}
		case 0: // Minkowski Difference
		case 1: // Minkowski Sum
		case 4: // GJK
		{
			showGrid = true;

			for (int i = 0; i < 6; i++)
			{
				Point p = Point(Random(1, 12), Random(1, 8));
				hull1.push_back(p);
			}
			for (int i = 0; i < 6; i++)
			{
				Point p = Point(Random(13, 24), Random(9, 16));
				hull2.push_back(p);
			}

			hull1Draw = Quickhull(&hull1);
			hull2Draw = Quickhull(&hull2);

			switch (selectedButton)
			{
			case 0:
			case 4:
			{
				resultHull = MinkowskiDifference(&hull1, &hull2);
				break;
			}
			case 1:
			{
				resultHull = MinkowskiSum(&hull1, &hull2);
				break;
			}
			}

			break;
		}
		}
		break;
	}
	case WM_LBUTTONDOWN:
	{
		GetCursorPos(&mousePos);
		ScreenToClient(hwnd, &mousePos);

		if (selectedButton == 3)
		{
			Point toGrid = PointToGrid(testPoint);
			if (abs(mousePos.x - toGrid.x) <= pointRadius * zoom && abs(mousePos.y - toGrid.y) <= pointRadius * zoom)
			{
				draggingPoint = &testPoint;
				pointInHull = 1;
				break;
			}
		}
		else
		{
			for (int i = 0; i < hull1.size(); i++)
			{
				Point toGrid = PointToGrid(hull1.at(i));
				if (abs(mousePos.x - toGrid.x) <= pointRadius * zoom && abs(mousePos.y - toGrid.y) <= pointRadius * zoom)
				{
					draggingPoint = &hull1.at(i);
					pointInHull = 1;
					break;
				}
			}

			for (int i = 0; i < hull2.size(); i++)
			{
				Point toGrid = PointToGrid(hull2.at(i));
				if (abs(mousePos.x - toGrid.x) <= pointRadius * zoom && abs(mousePos.y - toGrid.y) <= pointRadius * zoom)
				{
					draggingPoint = &hull2.at(i);
					pointInHull = 2;
					break;
				}
			}
		}

		// test if dragging hull
		if (selectedButton != -1)
		{
			Point p = GridToPoint(Point((float)mousePos.x, (float)mousePos.y));
			if (hull1Draw.size() > 0 && IsPointInside(&hull1Draw, p))
				draggingHull = &hull1;
			else if (hull2Draw.size() > 0 && IsPointInside(&hull2Draw, p))
				draggingHull = &hull2;
		}
		
		if (showGrid && draggingPoint == nullptr && draggingHull == nullptr && mousePos.x > uiSplit)
			draggingGrid = true;

		break;
	}
	case WM_LBUTTONUP:
	{
		draggingPoint = nullptr;
		draggingHull = nullptr;
		pointInHull = 0;
		draggingGrid = false;
		break;
	}
	case WM_MOUSEMOVE:
	{
		POINT pos;
		GetCursorPos(&pos);
		ScreenToClient(hwnd, &pos);

		cursorPoint = nullptr;

		if (selectedButton == 3)
		{
			Point toGrid = PointToGrid(testPoint);
			if (abs(pos.x - toGrid.x) <= pointRadius * zoom && abs(pos.y - toGrid.y) <= pointRadius * zoom)
			{
				cursorPoint = &testPoint;
			}
		}

		for (int i = 0; i < hull1.size(); i++)
		{
			Point toGrid = PointToGrid(hull1.at(i));
			if (abs(pos.x - toGrid.x) <= pointRadius * zoom && abs(pos.y - toGrid.y) <= pointRadius * zoom)
			{
				cursorPoint = &hull1.at(i);
				break;
			}
		}

		for (int i = 0; i < hull2.size(); i++)
		{
			Point toGrid = PointToGrid(hull2.at(i));
			if (abs(pos.x - toGrid.x) <= pointRadius * zoom && abs(pos.y - toGrid.y) <= pointRadius * zoom)
			{
				cursorPoint = &hull2.at(i);
				break;
			}
		}

		if (draggingPoint != nullptr)
		{
			Point newPos((float)pos.x, (float)pos.y);
			newPos = GridToPoint(newPos);

			*draggingPoint = newPos;

			if(pointInHull == 1)
				hull1Draw = Quickhull(&hull1);
			if(pointInHull == 2)
				hull2Draw = Quickhull(&hull2);

			switch (selectedButton)
			{
			case 0:
			case 4:
			{
				resultHull = MinkowskiDifference(&hull1, &hull2);
				break;
			}
			case 1:
			{
				resultHull = MinkowskiSum(&hull1, &hull2);
				break;
			}
			}
		}
		else if (draggingHull != nullptr)
		{
			Point pNew = GridToPoint(Point((float)pos.x, (float)pos.y));
			Point pMouse = GridToPoint(Point((float)mousePos.x, (float)mousePos.y));
			for (int i = 0; i < draggingHull->size(); i++)
			{
				draggingHull->at(i).x += pNew.x - pMouse.x;
				draggingHull->at(i).y += pNew.y - pMouse.y;
			}
			mousePos = pos;

			// Recalculate hulls
			if (draggingHull == &hull1) hull1Draw = Quickhull(&hull1);
			if (draggingHull == &hull2) hull2Draw = Quickhull(&hull2);

			switch (selectedButton)
			{
			case 0:
			case 4:
			{
				resultHull = MinkowskiDifference(&hull1, &hull2);
				break;
			}
			case 1:
			{
				resultHull = MinkowskiSum(&hull1, &hull2);
				break;
			}
			}
		}
		else if (draggingGrid)
		{
			offsetX += pos.x - mousePos.x;
			offsetY += pos.y - mousePos.y;
			mousePos = pos;
		}
		break;
	}
	case WM_MOUSEWHEEL:
	{
		float zDelta = (float)GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
		gridSize = max(gridSize + zDelta * zoomScale * gridSize, 16);
		zoom = (float)gridSize / defaultGridSize;
		break;
	}
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void Draw()
{
	// base UI
	graphics->Clear(0, 0, 0);

	if (showGrid)
	{
		// horizontal lines
		for (int i = 0; i * gridSize + ((originY + offsetY) % gridSize) < screenHeight; i++)
		{
			graphics->DrawLine(
				0,
				i * gridSize + ((originY + offsetY) % gridSize),
				screenWidth,
				i * gridSize + ((originY + offsetY) % gridSize),
				Color{ 95,95,95 }, 1
			);
		}

		// vertical lines
		for (int i = 0; (i * gridSize) + ((originX + offsetX) % gridSize) < screenWidth; i++)
		{
			graphics->DrawLine(
				(i * gridSize) + ((originX + offsetX) % gridSize),
				0,
				(i * gridSize) + ((originX + offsetX) % gridSize),
				screenHeight,
				Color{ 95,95,95, }, 1
			);
		}

		graphics->DrawLine(originX + offsetX, 0, originX + offsetX, screenHeight, Color{ 127,127,127 }, 4);		// Y-axis
		graphics->DrawLine(0, originY + offsetY, screenWidth, originY + offsetY, Color{ 127,127,127 }, 4);		// X-axis
	}

	// Draw hulls

	Color hull1Color(255,255,255);
	Color hull2Color(255,255,255);
	if (selectedButton == 4)
	{
		hull1Color = Color(255, 0, 0);
		hull2Color = Color(0, 0, 255);
	}

	// hull 1
	if (hull1.size() > 0)
	{
		for (int i = 0; i < hull1Draw.size(); i++)
		{
			Point p1 = PointToGrid(hull1Draw.at(i));
			Point p2 = PointToGrid(hull1Draw.at((i + 1) % hull1Draw.size()));
			graphics->DrawLine(p1.x, p1.y, p2.x, p2.y, hull1Color, 2);
		}
	}
	if (selectedButton != 3)
	{
		for (int i = 0; i < hull1.size(); i++)
		{
			Point p = PointToGrid(hull1.at(i));
			graphics->FillCircle(p.x, p.y, pointRadius * zoom, Color{ 0,128,0 });

			if (cursorPoint == &hull1.at(i))
				graphics->DrawCircle(p.x, p.y, pointRadius * zoom, Color{ 255,255,0 }, 2);
			else
				graphics->DrawCircle(p.x, p.y, pointRadius * zoom, hull1Color, 2);
		}

	}

	// hull 2
	if (hull2.size() > 0)
	{
		for (int i = 0; i < hull2Draw.size(); i++)
		{
			Point p1 = PointToGrid(hull2Draw.at(i));
			Point p2 = PointToGrid(hull2Draw.at((i + 1) % hull2Draw.size()));
			graphics->DrawLine(p1.x, p1.y, p2.x, p2.y, hull2Color, 2);
		}
	}
	for (int i = 0; i < hull2.size(); i++)
	{
		Point p = PointToGrid(hull2.at(i));
		graphics->FillCircle(p.x, p.y, pointRadius * zoom, Color{ 0,128,0 });

		if (cursorPoint == &hull2.at(i))
			graphics->DrawCircle(p.x, p.y, pointRadius * zoom, Color{ 255,255,0 }, 2);
		else
			graphics->DrawCircle(p.x, p.y, pointRadius * zoom, hull2Color, 2);
	}

	// result hull
	if (resultHull.size() > 0)
	{
		Color color = selectedButton == 4 && IsPointInside(&resultHull, Point(0, 0)) ? Color{ 0,255,0 } : Color{ 255,0,255 };
		for (int i = 0; i < resultHull.size(); i++)
		{
			Point p1 = PointToGrid(resultHull.at(i));
			Point p2 = PointToGrid(resultHull.at((i + 1) % resultHull.size()));
			graphics->DrawLine(p1.x, p1.y, p2.x, p2.y, color, 2);
		}
	}

	// Point for Point Convex Hull
	if (selectedButton == 3) {
		Point p = PointToGrid(testPoint);
		Color color = IsPointInside(&hull1Draw, testPoint) ? Color{ 255,0,0 } : Color{ 0,127,0 };
		graphics->FillCircle(p.x, p.y, pointRadius * zoom, color);
		if (cursorPoint == &testPoint)
			graphics->DrawCircle(p.x, p.y, pointRadius * zoom, Color{ 255,255,0 }, 2);
		else
			graphics->DrawCircle(p.x, p.y, pointRadius * zoom, hull1Color, 2);
	}

	// UI divider
	graphics->FillRect(0, 0, uiSplit, screenHeight, Color{ 0, 0, 0 });
	graphics->DrawLine(uiSplit - 4, 0, uiSplit - 4, screenHeight, Color{ 255,255,255 }, 4);

	if (selectedButton != -1)
		graphics->DrawRect(
			buttonMargin - 4,
			20 + (60 * selectedButton) - 4,
			buttonMargin - 4 + uiSplit - (2 * buttonMargin) + 8,
			40 + 20 + (60 * selectedButton) - 4 + 8,
			Color{ 127, 127, 127 }, 4
		);
}

HWND Button(LPCWSTR name, HMENU hMenu, float x, float y, float width, float height, HWND hwnd) {

	return CreateWindow(
		L"BUTTON",
		name,
		WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		x, y, width, height,
		hwnd, hMenu, NULL, NULL
	);

}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	WNDCLASSEX windowClass;
	ZeroMemory(&windowClass, sizeof(WNDCLASSEX));
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	windowClass.hInstance = hInstance;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.lpszClassName = L"MainWindow";
	windowClass.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClassEx(&windowClass);
	srand(time(NULL));


	// get screen size
	RECT screenRect = { 0, 0, 0, 0 };

	SystemParametersInfo(SPI_GETWORKAREA, NULL, &screenRect, NULL);

	screenWidth = screenRect.right - screenRect.left;
	screenHeight = screenRect.bottom - screenRect.top;

	originX = uiSplit + (screenWidth - uiSplit) / 2;
	originY = screenHeight / 2;

	maxX = (screenWidth - originX) / defaultGridSize - gridMargin;
	maxY = (screenHeight - originY) / defaultGridSize - gridMargin;

	// Adjust so that rect equals client size
	AdjustWindowRectEx(&screenRect, WS_OVERLAPPEDWINDOW, false, WS_EX_OVERLAPPEDWINDOW);

	HWND hwndMain = CreateWindowEx(
		0,
		windowClass.lpszClassName,
		L"ConvexHullAlgorithms",
		WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME | WS_CLIPCHILDREN,
		0, 0, screenWidth, screenHeight,
		NULL, NULL, hInstance, NULL
	);
	if (!hwndMain) return -1;

	// buttons
	buttons[0] = Button(L"Minkowski Difference", (HMENU)0, buttonMargin, 20, uiSplit - (2 * buttonMargin), 40, hwndMain);
	buttons[1] = Button(L"Minkowski Sum", (HMENU)1, buttonMargin, 80, uiSplit - (2 * buttonMargin), 40, hwndMain);
	buttons[2] = Button(L"Quickhull", (HMENU)2, buttonMargin, 140, uiSplit - (2 * buttonMargin), 40, hwndMain);
	buttons[3] = Button(L"Point Convex Hull", (HMENU)3, buttonMargin, 200, uiSplit - (2 * buttonMargin), 40, hwndMain);
	buttons[4] = Button(L"GJK", (HMENU)4, buttonMargin, 260, uiSplit - (2 * buttonMargin), 40, hwndMain);

	graphics = new Graphics();
	if (!graphics->Init(hwndMain))
	{
		delete graphics;
		return -1;
	}

	ShowWindow(hwndMain, nCmdShow);

	MSG message{};
	message.message = WM_NULL;

	while (message.message != WM_QUIT)
	{
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			DispatchMessage(&message);
		}
		else
		{
			graphics->BeginDraw();

			Draw();

			graphics->EndDraw();
			UpdateWindow(hwndMain);
		}
	}

	delete graphics;

	return 0;
}