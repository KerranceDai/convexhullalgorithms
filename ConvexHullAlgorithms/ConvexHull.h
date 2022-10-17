#pragma once

#include <vector>
#include "Vector.h";
#include "Point.h";
using namespace std;

int GetIndex(vector<Point>* v, Point p);

float DotProduct(Vector v1, Vector v2);

vector<Point> Quickhull(vector<Point>* original);

vector<Point> MinkowskiDifference(vector<Point>* hull1, vector<Point>* hull2);

vector<Point> MinkowskiSum(vector<Point>* hull1, vector<Point>* hull2);

bool IsPointInside(vector<Point>* hull1, Point point);