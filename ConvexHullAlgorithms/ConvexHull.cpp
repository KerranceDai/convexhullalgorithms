#include "ConvexHull.h"

int GetIndex(vector<Point>* v, Point p)
{
	for (int i = 0; i < v->size(); i++) {

		Point pTemp = v->at(i);
		if (pTemp.x == p.x && pTemp.y == p.y) {
			return i;
		}
	}
	return -1;
}

float DotProduct(Vector v1, Vector v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}

vector<Point> Quickhull(vector<Point>* original)
{
	vector<Point> hull;

	Point topMost = original->at(0);
	Point leftMost = topMost;
	Point bottomMost = topMost;
	Point rightMost = topMost;

	// get extremes
	for (int i = 0; i < original->size(); i++)
	{
		Point* p = &original->at(i);
		if (p->x > rightMost.x)
			rightMost = *p;
		if (p->x < leftMost.x)
			leftMost = *p;
		if (p->y > topMost.y)
			topMost = *p;
		if (p->y < bottomMost.y)
			bottomMost = *p;
	}

	hull.push_back(topMost);

	if (GetIndex(&hull, leftMost) == -1)
		hull.push_back(leftMost);

	if (GetIndex(&hull, bottomMost) == -1)
		hull.push_back(bottomMost);

	if (GetIndex(&hull, rightMost) == -1)
		hull.push_back(rightMost);

	for (int i = 0; i < hull.size(); i++)
	{
		Point* p1 = &hull.at(i);
		Point* p2 = &hull.at((i + 1) % hull.size());

		Vector line(*p1, *p2);
		line = line.Normalize();

		Vector perp = line.Perpendicular();

		int furthestIndex = -1;
		float furthestDotProduct = 0;

		for (int j = 0; j < original->size(); j++)
		{
			Vector pointToPoint1(original->at(j), *p1);
			pointToPoint1 = pointToPoint1.Normalize();

			float dotProduct = DotProduct(pointToPoint1, perp);

			if (dotProduct > furthestDotProduct && GetIndex(&hull, original->at(j)) == -1)
			{
				furthestDotProduct = dotProduct;
				furthestIndex = j;
			}
		}

		if (furthestIndex != -1)  // furthest point found
		{
			if (i == hull.size() - 1)
				hull.push_back(original->at(furthestIndex));
			else
				hull.insert(hull.begin() + i + 1, original->at(furthestIndex));

			i -= 1;
		}
	}

	return hull;
}

vector<Point> MinkowskiDifference(vector<Point>* hull1, vector<Point>* hull2)
{
	vector<Point> resultHull;
	for (int i = 0; i < hull1->size(); i++)
	{
		Point point1 = hull1->at(i);

		for (int j = 0; j < hull2->size(); j++)
		{
			Point point2 = hull2->at(j);
			Point newPoint;
			newPoint.x = point1.x - point2.x;
			newPoint.y = point1.y - point2.y;
			resultHull.push_back(newPoint);
		}
	}

	resultHull = Quickhull(&resultHull);

	return resultHull;
}

vector<Point> MinkowskiSum(vector<Point>* hull1, vector<Point>* hull2)
{
	vector<Point> resultHull;
	for (int i = 0; i < hull1->size(); i++)
	{
		Point point1 = hull1->at(i);

		for (int j = 0; j < hull2->size(); j++)
		{
			Point point2 = hull2->at(j);
			Point newPoint;
			newPoint.x = point1.x + point2.x;
			newPoint.y = point1.y + point2.y;
			resultHull.push_back(newPoint);
		}
	}

	resultHull = Quickhull(&resultHull);

	return resultHull;
}

bool IsPointInside(vector<Point>* hull, Point p)
{
	for (int i = 0; i < hull->size(); i++)
	{
		Point p1 = hull->at(i);
		Point p2 = hull->at((i + 1) % hull->size());

		Vector line(p1, p2);
		Vector perpendicular = line.Perpendicular();

		Vector pointToPoint1;
		pointToPoint1.x = p.x - p1.x;
		pointToPoint1.y = p.y - p1.y;

		Vector normalizedPointToPoint1 = pointToPoint1.Normalize();
		Vector normalizedPerpendicular = perpendicular.Normalize();

		float dotProduct = DotProduct(normalizedPerpendicular, normalizedPointToPoint1);

		if (dotProduct < 0)
			return false;
	}

	return true;
}